#include <core/RK/RKSystem.hpp>
#include <core/nw4r/ut/Misc.hpp>
#include <MarioKartWii/Scene/RootScene.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <PulsarSystem.hpp>
#include <Extensions/LECODE/LECODEMgr.hpp>
#include <Gamemodes/KO/KOMgr.hpp>
#include <Gamemodes/KO/KOHost.hpp>
#include <Gamemodes/OnlineTT/OnlineTT.hpp>
#include <Settings/Settings.hpp>
#include <Config.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <core/egg/DVD/DvdRipper.hpp>
namespace Pulsar {

System* System::sInstance = nullptr;
System::Inherit* System::inherit = nullptr;

void System::CreateSystem() {
    if(sInstance != nullptr) return;
    EGG::Heap* heap = RKSystem::mInstance.EGGSystem;
    const EGG::Heap* prev = heap->BecomeCurrentHeap();
    System* system;
    if(inherit != nullptr) {
        system = inherit->create();
    }
    else system = new System();
    System::sInstance = system;
    ConfigFile& conf = ConfigFile::LoadConfig();
    system->Init(conf);
    prev->BecomeCurrentHeap();
    conf.Destroy();
}
//kmCall(0x80543bb4, System::CreateSystem);
BootHook CreateSystem(System::CreateSystem, 0);

System::System() :
    heap(RKSystem::mInstance.EGGSystem), taskThread(EGG::TaskThread::Create(8, 0, 0x4000, this->heap)),
    //Modes
    koMgr(nullptr), ottHideNames(false) {
}

void System::Init(const ConfigFile& conf) {
    IOType type = IOType_ISO;
    s32 ret = IO::OpenFix("file", IOS::MODE_NONE);

    if(ret >= 0) {
        type = IOType_RIIVO;
        IOS::Close(ret);
    }
    else {
        ret = IO::OpenFix("/dev/dolphin", IOS::MODE_NONE);
        if(ret >= 0) {
            type = IOType_DOLPHIN;
            IOS::Close(ret);
        }
    }
    strncpy(this->modFolderName, conf.header.modFolderName, IOS::ipcMaxFileName);

    //InitInstances
    CupsConfig::sInstance = new CupsConfig(conf.GetSection<CupsHolder>());
    this->info.Init(conf.GetSection<InfoHolder>().info);
    this->InitIO(type);
    this->InitSettings(&conf.GetSection<CupsHolder>().trophyCount[0]);


    //Initialize last selected cup and courses
    const PulsarCupId last = Settings::Mgr::sInstance->GetSavedSelectedCup();
    CupsConfig* cupsConfig = CupsConfig::sInstance;
    cupsConfig->SetLayout();
    if(last != -1 && cupsConfig->IsValidCup(last) && cupsConfig->GetTotalCupCount() > 8) {
        cupsConfig->lastSelectedCup = last;
        cupsConfig->SetSelected(cupsConfig->ConvertTrack_PulsarCupToTrack(last, 0));
        cupsConfig->lastSelectedCupButtonIdx = last & 1;
    }

    //Track blocking 
    u32 trackBlocking = this->info.GetTrackBlocking();
    this->netMgr.lastTracks = new PulsarId[trackBlocking];
    for(int i = 0; i < trackBlocking; ++i) this->netMgr.lastTracks[i] = PULSARID_NONE;
    const BMGHeader* const confBMG = &conf.GetSection<PulBMG>().header;
    this->rawBmg = EGG::Heap::alloc<BMGHeader>(confBMG->fileLength, 0x4, RootScene::sInstance->expHeapGroup.heaps[1]);
    memcpy(this->rawBmg, confBMG, confBMG->fileLength);
    this->customBmgs.Init(*this->rawBmg);
    this->AfterInit();
}

//IO
#pragma suppress_warnings on
void System::InitIO(IOType type) const {

    IO* io = IO::CreateInstance(type, this->heap, this->taskThread);
    bool ret;
    if(io->type == IOType_DOLPHIN) ret = ISFS::CreateDir("/shared2/Pulsar", 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
    const char* modFolder = this->GetModFolder();
    ret = io->CreateFolder(modFolder);
    if(!ret && io->type == IOType_DOLPHIN) {
        char path[0x100];
        snprintf(path, 0x100, "Unable to automatically create a folder for this CT distribution\nPlease create a Pulsar folder in Dolphin Emulator/Wii/shared2", modFolder);
        Debug::FatalError(path);
    }
    char ghostPath[IOS::ipcMaxPath];
    snprintf(ghostPath, IOS::ipcMaxPath, "%s%s", modFolder, "/Ghosts");
    io->CreateFolder(ghostPath);
}
#pragma suppress_warnings reset

void System::InitSettings(const u16* totalTrophyCount) const {
    Settings::Mgr* settings = new (this->heap) Settings::Mgr;
    char path[IOS::ipcMaxPath];
    snprintf(path, IOS::ipcMaxPath, "%s/%s", this->GetModFolder(), "Settings.pul");
    settings->Init(totalTrophyCount, path); //params
    Settings::Mgr::sInstance = settings;
}

void System::UpdateContext() {
    const RacedataSettings& racedataSettings = Racedata::sInstance->menusScenario.settings;
    this->ottVoteState = OTT::COMBO_NONE;
    const Settings::Mgr& settings = Settings::Mgr::Get();

    // Radio Contexts
    bool isCT = true;
    bool isPUL = true;
    u8   hostWins = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS);
    bool isKO = false;
    bool isOTT = settings.GetSettingValue(Settings::SETTINGSTYPE_OTTKO, SETTINGOTT_VERSUS);
    bool isMiiHeads = settings.GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_MII);
    bool isDisregard = false;
    u8   turboStyle = settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_TURBO);
    u8   lapMaths = settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_RADIO_CALC);
    bool isItemStart = settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_RADIO_STARTENABLED);
    bool isBrake = 1; // At this stage, this value means "Is Brake Drifting Allowed?" since it needs to be calculated later

    // Large Contexts
    u8 lapsLaps = settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_SCROLL_LAPS);
    u8 physSpeed = settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_SPEED);
    u8 physGravity = settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_GRAV);
    u8 itemRoulette = settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_ROULETTE);
    u8 itemStart = settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START);
    u8 itemCloudEffect = settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_CLOUD);
    u8 physVehicleStats = settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_VEHICLESTATS);

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const GameMode mode = racedataSettings.gamemode;
    Network::Mgr& netMgr = this->netMgr;
    const u32 sceneId = GameScene::GetCurrent()->id;

    // Config.pul contexts:
    //bool is200 = racedataSettings.engineClass == CC_100 && this->info.Has200cc();
    //bool isFeather = this->info.HasFeather();
    //bool isUMTs = this->info.HasUMTs();
    //bool isMegaTC = this->info.HasMegaTC();

    u32 newRadioContexts = 0;
    u8 newLapsLaps = 0;
    u8 newSpeedMod = 0;
    u8 newGravMod = 0;
    u8 newRouletteBin = 0;
    u8 newKartBin = 0;
    u8 newItemStart = 0;
    u8 newCloudEffect = 0;

    if(sceneId != SCENE_ID_GLOBE && controller->connectionState != RKNet::CONNECTIONSTATE_SHUTDOWN) {
        switch(controller->roomType) {
            case(RKNet::ROOMTYPE_VS_REGIONAL): // Reset gameplay altering settings to default to keep regionals healthy!

                // Variable based on config
                switch(racedataSettings.engineClass) {
                    case(CC_100):
                        if(this->info.Has200cc()) physSpeed = PHYSSETTING_SPEED_150;
                        else physSpeed = PHYSSETTING_SPEED_100; // Not entirely sure how this works
                        break;
                    case(CC_150): physSpeed = PHYSSETTING_SPEED_100 ;break;
                    //case(CC_MIRROR): physSpeed = PHYSSETTING_SPEED_100 ;break;
                    case(CC_BATTLE): physSpeed = PHYSSETTING_SPEED_100 ;break;
                    default: physSpeed = PHYSSETTING_SPEED_100;
                }
                if(this->info.HasMegaTC()) itemCloudEffect = ITEMSETTING_CLOUD_MEGA;
                else itemCloudEffect = ITEMSETTING_CLOUD_SHOCK;
                if(this->info.HasUMTs()) turboStyle = PHYSSETTING_TURBO_UMT;
                else turboStyle = PHYSSETTING_TURBO_VANILLA;

                // Constant
                isBrake = true; // Is Brake Drift Allowed?
                lapMaths = LAPSETTING_CALC_MATHS;
                lapsLaps = LAPSETTING_LAPS_3;
                physGravity = PHYSSETTING_GRAVITY_100;
                itemRoulette = ITEMSETTING_ROULETTE_STANDARD;
                physVehicleStats = PHYSSETTING_KARTSTAT_VANILLA;
                isItemStart = ITEMSETTING_START_DISABLED;
                itemStart = ITEMSETTING_START_3MUS;

                break;
            case(RKNet::ROOMTYPE_JOINING_REGIONAL):
                isOTT = netMgr.ownStatusData == true;
                break;
            case(RKNet::ROOMTYPE_FROOM_HOST):
                // sudo rm -rf /*
                break;
            case(RKNet::ROOMTYPE_FROOM_NONHOST):
                isCT = mode != MODE_BATTLE && mode != MODE_PUBLIC_BATTLE && mode != MODE_PRIVATE_BATTLE;

                // Net Contexts
                newRadioContexts = netMgr.hostRadioContexts;
                newLapsLaps = netMgr.hostLapCount;
                newSpeedMod = netMgr.hostSpeedMod;
                newGravMod = netMgr.hostGravMod;
                newRouletteBin = netMgr.hostRouletteBin;
                newKartBin = netMgr.hostKartBin;
                newItemStart = netMgr.hostStartItem;
                newCloudEffect = netMgr.hostCloudEffect;

                // Define disregard early so it does what it's meant to
		isDisregard = newRadioContexts & ( 1 << HOST_DISREGARD );

                // Always Enabled
                hostWins = newRadioContexts & (1 << HOST_HAW);
                isKO = newRadioContexts & (1 << MODE_KO);
                isMiiHeads = newRadioContexts & (1 << RACE_MIIHEADS);

                if (isDisregard != HOSTSETTING_DISREGARD_ENABLED) { // Settings that don't always NEED to be synced:

                    // Radio Contexts
                    isOTT = newRadioContexts & (1 << MODE_OTT);
                    turboStyle = newRadioContexts & (1 << PHYS_TURBO);
                    isBrake = newRadioContexts & (1 << PHYS_BRAKE); // Is Brake Drift Allowed? take host settings
                    lapMaths = newRadioContexts & (1 << LAP_MATHS);
                    itemStart = newRadioContexts & (1 << ITEM_START_ENABLED);

                    // Large Contexts
                    lapsLaps = newLapsLaps;
                    physSpeed = newSpeedMod;
                    physGravity = newGravMod;
                    itemRoulette = newRouletteBin;
                    itemStart = newItemStart;
                    itemCloudEffect = newCloudEffect;
                    physVehicleStats = newKartBin;

                } // Disregard Not Enabled

                break;
            default:
                isCT = false;
                isPUL = false;
        }
    }

    else {
//        const u8 ottOffline = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_VERSUS);
//        isOTT = (mode == MODE_GRAND_PRIX || mode == MODE_VS_RACE) ? (ottOffline == OTTSETTING_ENABLED) : false; //offlineOTT
//        isOTT = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_VERSUS);
//        if(isOTT) {
//            isFeather &= (ottOffline == OTTSETTING_OFFLINE_FEATHER);
//            isUMTs &= ~settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ALLOWUMTS);
//        }
    }

    this->netMgr.hostRadioContexts = newRadioContexts;
    this->netMgr.hostLapCount = newLapsLaps;
    this->netMgr.hostSpeedMod = newSpeedMod;
    this->netMgr.hostGravMod = newGravMod;
    this->netMgr.hostRouletteBin = newRouletteBin;
    this->netMgr.hostKartBin = newKartBin;
    this->netMgr.hostStartItem = newItemStart;
    this->netMgr.hostCloudEffect = newCloudEffect;

    // Brake Drift needs to be calculated after room settings since it differs based on several settings
    if (isBrake) {
        switch (settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_BRAKE)) {
            case PHYSSETTING_BRAKE_ENABLED: isBrake = true; break;
            case PHYSSETTING_BRAKE_STANDARD:
                if ( physSpeed >= PHYSSETTING_SPEED_125
                   & physSpeed <= PHYSSETTING_SPEED_999
                   | physSpeed >= PHYSSETTING_GRAVITY_025
                   & physSpeed <= PHYSSETTING_GRAVITY_075
                   ) { isBrake = true; } break;
            default: isBrake = false;
        } // Switch Local Brake Setting
    } // If Brake Drift Allowed

    // Validate Time Trials
    // Previous way of doing this was very awkward to edit - this isn't much better but I still can't think of a good way of doing this

    bool isValidTT = 1;
    isValidTT &= lapMaths == LAPSETTING_CALC_MATHS || lapMaths == LAPSETTING_CALC_EXCLUDE;
    isValidTT &= lapsLaps == LAPSETTING_LAPS_3;
    isValidTT &= physGravity == PHYSSETTING_GRAVITY_100;
    isValidTT &= ! isBrake // Brake Drift XNOR 200cc type classes
                 ^ physSpeed >= PHYSSETTING_SPEED_125
                 & physSpeed <= PHYSSETTING_SPEED_999;
    isValidTT &= turboStyle == this->info.HasUMTs();
    isValidTT &= physVehicleStats == PHYSSETTING_KARTSTAT_VANILLA;

    // Extra validity notes:
    // - All speedmods are valid now since they're split up into categories, so we don't need to check for that
    // - All startitems are also valid for the same reason


    // Contexts
    // isCT  - Whether CTs are enabled (Disabled in battle)
    // isPUL - Only ever disabled for RTWWs

    // Initial definitions - Enabled in RTWWs!
    u32 contextRadioContexts = ( isCT << PULSAR_CT )
                      | ( isMiiHeads << RACE_MIIHEADS );
    u8 contextLapsLaps = LAPSETTING_LAPS_3;
    u8 contextSpeedMod = PHYSSETTING_SPEED_100;
    u8 contextGravMod = PHYSSETTING_GRAVITY_100;
    u8 contextRouletteBin = ITEMSETTING_ROULETTE_VANILLA;
    u8 contextKartBin = PHYSSETTING_KARTSTAT_VANILLA;
    u8 contextItemStart = ITEMSETTING_START_NONE; // Not important
    u8 contextCloudEffect = ITEMSETTING_CLOUD_SHOCK;

    // if CTs are enabled
    if (isCT) {
        contextRadioContexts |= ( isOTT << MODE_OTT )
                       | ( lapMaths << LAP_MATHS );
        contextLapsLaps = lapsLaps;
    } // isCT

    // if anything but RTWWs
    if (isPUL) {
        contextRadioContexts |= ( hostWins << HOST_HAW )
                       | ( isKO << MODE_KO )
                       | ( isBrake << PHYS_BRAKE )
                       | ( turboStyle << PHYS_TURBO )
                       | ( isDisregard << HOST_DISREGARD )
                       | ( isItemStart << ITEM_START_ENABLED );
        contextSpeedMod = physSpeed;
        contextGravMod = physGravity;
        contextRouletteBin = itemRoulette;
        contextItemStart = itemStart;
        contextCloudEffect = itemCloudEffect;
    } // isPUL

    this->radioContexts = contextRadioContexts;
    this->lapCount = contextLapsLaps;
    this->speedMod = contextSpeedMod;
    this->gravMod = contextGravMod;
    this->rouletteBin = contextRouletteBin;
    this->kartBin = contextKartBin;
    this->itemStart = contextItemStart;
    this->cloudEffect = contextCloudEffect;

    //Create temp instances if needed:
    /*
    if(sceneId == SCENE_ID_RACE) {
        if(this->lecodeMgr == nullptr) this->lecodeMgr = new (this->heap) LECODE::Mgr;
    }
    else if(this->lecodeMgr != nullptr) {
        delete this->lecodeMgr;
        this->lecodeMgr = nullptr;
    }
    */

    if(isKO) {
        if(sceneId == SCENE_ID_MENU && SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber == -1) this->koMgr = new (this->heap) KO::Mgr; //create komgr when loading the select phase of the 1st race of a froom
    }
    if(!isKO && this->koMgr != nullptr || isKO && sceneId == SCENE_ID_GLOBE) {
        delete this->koMgr;
        this->koMgr = nullptr;
    }
}

s32 System::OnSceneEnter(Random& random) {
    System* self = System::sInstance;
    self->UpdateContext();
    if(self->GetBoolRadioContext(MODE_OTT)) OTT::AddGhostToVS();
    if(self->GetFullRadioContext(HOST_HAW) == HOSTSETTING_HOSTWINS_ENABLED && self->GetBoolRadioContext(MODE_KO) && GameScene::GetCurrent()->id == SCENE_ID_RACE && SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber > 0) {
        KO::HAWChangeData();
    }
    return random.NextLimited(8);
}
kmCall(0x8051ac40, System::OnSceneEnter);

asmFunc System::GetRaceCount() {
    ASM(
        nofralloc;
    lis r5, sInstance@ha;
    lwz r5, sInstance@l(r5);
    lbz r0, System.netMgr.racesPerGP(r5);
    blr;
        )
}

asmFunc System::GetNonTTGhostPlayersCount() {
    ASM(
        nofralloc;
    lis r12, sInstance@ha;
    lwz r12, sInstance@l(r12);
    lbz r29, System.nonTTGhostPlayersCount(r12);
    blr;
        )
}

//Unlock Everything Without Save (_tZ)
kmWrite32(0x80549974, 0x38600001);

//Skip ESRB page
kmRegionWrite32(0x80604094, 0x4800001c, 'E');

const char System::pulsarString[] = "/Pulsar";
const char System::CommonAssets[] = "/CommonAssets.szs";
const char System::breff[] = "/Effect/Pulsar.breff";
const char System::breft[] = "/Effect/Pulsar.breft";
const char* System::ttModeFolders[] ={ "150", "200", "150F", "200F", "Unrestricted" };

}//namespace Pulsar