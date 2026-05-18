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
#include <BlFa3/VariousUtilityFunctions.hpp>
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

    // try not to exceed 24 context bits because we think it causes problems
    // pulsar divider -- 11 context bits

    bool isCT = true;
    bool isLOL = true;
    bool isHAW = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) > 0;
    bool lolHAW = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) > 1;
    bool isKO = false;
    bool isOTT = settings.GetSettingValue(Settings::SETTINGSTYPE_OTTKO, SETTINGOTT_VERSUS);
    bool isMiiHeads = settings.GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_MII);
    bool cdpDisregard = false;
    bool isUMTs1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_TURBO),1);
    bool isUMTs2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_TURBO),2);

    // lolpack divider -- 23+1 context bits (TTVALID)

    bool lolLapType1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_RADIO_CALC),1);
    bool lolLapType2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_RADIO_CALC),2);
    bool lolLaps1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_SCROLL_LAPS),1);
    bool lolLaps2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_SCROLL_LAPS),2);
    bool lolLaps3 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_SCROLL_LAPS),3);
    bool lolLaps4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_LAP, SETTINGLAP_SCROLL_LAPS),4);
    bool lolSpeeds1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_SPEED),1);
    bool lolSpeeds2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_SPEED),2);
    bool lolSpeeds3 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_SPEED),3);
    bool lolSpeeds4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_SPEED),4);
    bool cdpGravity1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_GRAV),1);
    bool cdpGravity2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_GRAV),2);
    bool cdpGravity3 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_GRAV),3);
    bool cdpGravity4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_GRAV),4);
    // It's important to define the last setting of lolRoulette as 0, so add one and mod by highest setting value.
    bool lolRoulette1 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_ROULETTE)+1)%6,1);
    bool lolRoulette2 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_ROULETTE)+1)%6,2);
    bool lolRoulette3 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_ROULETTE)+1)%6,3);
    bool lolTTitem1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START),1);
    bool lolTTitem2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START),2);
    bool lolTTitem3 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START),3);
    bool lolTTitem4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START),4);
    bool lolTTitem5 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_START),5);
    bool lolTTitembool = settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_RADIO_STARTENABLED);

    // wdd & cdp divider -- 4 context bits

    bool wddtceffect1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_CLOUD),1);
    bool wddtceffect2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_CLOUD),2);
    bool wddtceffect3 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_CLOUD),3);
    bool wddtceffect4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_ITEM, SETTINGITEM_SCROLL_CLOUD),4);
    //bool cdpVehicleStats = settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_SCROLL_VEHICLESTATS); // Needs actual settings - hurry up and work on those vehicle stat edits!

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const GameMode mode = racedataSettings.gamemode;
    Network::Mgr& netMgr = this->netMgr;
    const u32 sceneId = GameScene::GetCurrent()->id;

    //bool is200 = racedataSettings.engineClass == CC_100 && this->info.Has200cc(); //unused
    bool isFeather = this->info.HasFeather();
    //bool isUMTs = this->info.HasUMTs(); // The normal definition for umts is undesirable now that we have trophy validation
    //bool isMegaTC = this->info.HasMegaTC(); // Unused because of settings

    bool isLolBrake = 1; // At this stage, this value means "Is Brake Drifting Allowed?" since it needs to be calculated later

    u32 newContextPul = 0;
    u32 newContextLOL = 0;
    u32 newContextWDD = 0;

    if(sceneId != SCENE_ID_GLOBE && controller->connectionState != RKNet::CONNECTIONSTATE_SHUTDOWN) {
        switch(controller->roomType) {
            case(RKNet::ROOMTYPE_VS_REGIONAL): // Reset gameplay altering settings to default to keep regionals healthy!
                isLolBrake = 1; // Is Brake Drift Allowed? (I'll say yes because I'm nice!)
                // Calculated Laps
                lolLapType1 = 0;
                lolLapType2 = 0;
                // 3 Laps
                lolLaps1 = 0;
                lolLaps2 = 0;
                lolLaps3 = 0;
                lolLaps4 = 0;
                // 1.0x Speed
                lolSpeeds1 = 0;
                lolSpeeds2 = 0;
                lolSpeeds3 = 0;
                lolSpeeds4 = 0;
                // 1.0x Gravity
                cdpGravity1 = 0;
                cdpGravity2 = 0;
                cdpGravity3 = 0;
                cdpGravity4 = 0;
                // Standard Roulette
                lolRoulette1 = 1;
                lolRoulette2 = 0;
                lolRoulette3 = 0;
                // Thundercloud // Take MegaTC from pack creator settings
                wddtceffect1 = this->info.HasMegaTC();
                wddtceffect2 = 0;
                wddtceffect3 = 0;
                wddtceffect4 = 0;
                // Starting Items
                lolTTitembool = 0;
                // Starting item // Unconditionally used in OTT regionals, so it's important to set this!
                lolTTitem1 = 0;
                lolTTitem2 = 0;
                lolTTitem3 = 0;
                lolTTitem4 = 0;
                lolTTitem5 = 0;
                // Turbo Style // Take UMTs from pack creator settings
                isUMTs1 = this->info.HasUMTs();
                isUMTs2 = 0;
                break;
            case(RKNet::ROOMTYPE_JOINING_REGIONAL):
                isOTT = netMgr.ownStatusData == true;
                break;
            case(RKNet::ROOMTYPE_FROOM_HOST):
                break;
            case(RKNet::ROOMTYPE_FROOM_NONHOST):
                isCT = mode != MODE_BATTLE && mode != MODE_PUBLIC_BATTLE && mode != MODE_PRIVATE_BATTLE;
                newContextPul = netMgr.hostContextPul;
                newContextLOL = netMgr.hostContextLOL;
                newContextWDD = netMgr.hostContextWDD;
                // Define disregard early so it does what it's meant to
		cdpDisregard = newContextPul & ( 1 << CDP_DISREGARD );
                isHAW = newContextPul & (1 << PULSAR_HAW_1);
                isKO = newContextPul & (1 << PULSAR_MODE_KO);
                isMiiHeads = newContextPul & (1 << PULSAR_MIIHEADS);
                if (!cdpDisregard) { // Settings that don't always NEED to be synced:
                    isOTT = newContextPul & (1 << PULSAR_MODE_OTT);
                    isUMTs1 = newContextPul & (1 << PHYS_TURBO_1); // Usually defined alongside feathers for ott, but we don't want that
                    isUMTs2 = newContextPul & (1 << PHYS_TURBO_2);
                    isLolBrake = newContextLOL & (1 << LOLPACK_BRAKE); // Is Brake Drift Allowed? take host settings
                    //lol settings
                    lolLapType1 = newContextLOL & (1 << LAP_MATHS_1);
                    lolLapType2 = newContextLOL & (1 << LAP_MATHS_2);
                    lolLaps1 = newContextLOL & (1 << LAP_COUNT_1);
                    lolLaps2 = newContextLOL & (1 << LAP_COUNT_2);
                    lolLaps3 = newContextLOL & (1 << LAP_COUNT_4);
                    lolLaps4 = newContextLOL & (1 << LAP_COUNT_8);
                    lolSpeeds1 = newContextLOL & (1 << PHYS_SPEED_1);
                    lolSpeeds2 = newContextLOL & (1 << PHYS_SPEED_2);
                    lolSpeeds3 = newContextLOL & (1 << PHYS_SPEED_4);
                    lolSpeeds4 = newContextLOL & (1 << PHYS_SPEED_8);
                    lolRoulette1 = newContextLOL & (1 << ITEM_ROULETTE_1);
                    lolRoulette2 = newContextLOL & (1 << ITEM_ROULETTE_2);
                    lolRoulette3 = newContextLOL & (1 << ITEM_ROULETTE_4);
                    lolTTitembool = newContextLOL & (1 << ITEM_START_ENABLED);
                    lolTTitem1 = newContextLOL & (1 << ITEM_START_1);
                    lolTTitem2 = newContextLOL & (1 << ITEM_START_2);
                    lolTTitem3 = newContextLOL & (1 << ITEM_START_4);
                    lolTTitem4 = newContextLOL & (1 << ITEM_START_8);
                    lolTTitem5 = newContextLOL & (1 << ITEM_START_16);
                    //end of lol settings
                    //wdd tc settings
                    //wddtcbehave = newContextWDD & (1 << ITEM_CLOUD_BEHAVE);
                    wddtceffect1 = newContextWDD & (1 << ITEM_CLOUD_1);
                    wddtceffect2 = newContextWDD & (1 << ITEM_CLOUD_2);
                    wddtceffect3 = newContextWDD & (1 << ITEM_CLOUD_4);
                    wddtceffect4 = newContextWDD & (1 << ITEM_CLOUD_8);
                    }//end of tc
                break;
            default:
                isCT = false;
                isLOL = false;
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
    this->netMgr.hostContextPul = newContextPul;
    this->netMgr.hostContextLOL = newContextLOL;
    this->netMgr.hostContextWDD = newContextWDD;

    // Brake Drift needs to be calculated after room settings since it differs based on several settings
    isLolBrake &= (BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_BRAKE),2)| // If setting = 1X, always enable
                  (!BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_PHYSICS, SETTINGPHYS_RADIO_BRAKE),1) // If setting = 01, disable. Else, check speedmod.
                  & (lolSpeeds1+lolSpeeds2*2+lolSpeeds3*4+lolSpeeds4*8 > 1) // Lower bound 2 = 1.25x speed
                  & (lolSpeeds1+lolSpeeds2*2+lolSpeeds3*4+lolSpeeds4*8 < 9) // Upper bound 8 = 100x speed
                  )); // AND assignment since we need to know if brakedrifting is allowed

    // Validate Time Trials
    bool isValidTT = ! // NOT gate result
                     //Valid lapcount would be 0000, = 3. (It'd be better to check directly with kmp, but not possible for most menus.)
                     (lolLaps1 |lolLaps2 |lolLaps3 |lolLaps4
                     // Valid gravity is 0000
                     |cdpGravity1 |cdpGravity2 |cdpGravity3 |cdpGravity4
                     //Calculated and Exclusive laps are always correct for 3 laps, but Forced laps cause issues, so restrict it.
                     |lolLapType2
                     //if speed is high, ignore as this is enabled anyway.
                     |(isLolBrake
                         ^((lolSpeeds1+lolSpeeds2*2+lolSpeeds3*4+lolSpeeds4*8 > 1) // Lower bound 2 = 1.25x speed
                         & (lolSpeeds1+lolSpeeds2*2+lolSpeeds3*4+lolSpeeds4*8 < 9) // Upper bound 8 = 100x speed
                     ))
                     // Verify UMT setting matches pack setting
                     |(this->info.HasUMTs() != isUMTs1)
                     |isUMTs2 // high setting check
                     // We run into a slight problem here - TT items depend on the mode, selected via menu buttons
                     // Eventual solution - Condense all buttons into settings
                     // Temporary solution - Make all buttons do the same thing lmao
                     );

// Extra validity notes:
// - All speedmods are valid now since they're split up into categories, so we don't need to check for that
// - All items are also valid for the same reason



    u32 contextPul = (isCT << PULSAR_CT)
                | (isMiiHeads << PULSAR_MIIHEADS);
    if(isCT) { contextPul //contexts that should only exist when CTs are on
                |=(isOTT << PULSAR_MODE_OTT);
    }
    if(isLOL) { contextPul //contexts that should only ever not exist for RTWWs
                |=(isLolBrake << LOLPACK_BRAKE)
                | (isHAW << PULSAR_HAW_1)
                | (isHAW << PULSAR_HAW_2)
                | (isFeather << PULSAR_FEATHER)
                | (isKO << PULSAR_MODE_KO)
                | (isUMTs1 << PHYS_TURBO_1)
                | (isUMTs2 << PHYS_TURBO_2)
                | (cdpDisregard << CDP_DISREGARD);
    }
    // LOL Contexts
    u32 contextLOL = (isValidTT << TTS_VALID);
    if(isCT) { contextLOL //contexts that should only exist when CTs are on
                |=(lolLapType1 << LAP_MATHS_1)
                | (lolLapType2 << LAP_MATHS_2)
                | (lolLaps1 << LAP_COUNT_1)
                | (lolLaps2 << LAP_COUNT_2)
                | (lolLaps3 << LAP_COUNT_4)
                | (lolLaps4 << LAP_COUNT_8);
    }
    if(isLOL) { contextLOL //contexts that should only ever not exist for RTWWs
                |=(lolSpeeds1 << PHYS_SPEED_1)
                | (lolSpeeds2 << PHYS_SPEED_2)
                | (lolSpeeds3 << PHYS_SPEED_4)
                | (lolSpeeds4 << PHYS_SPEED_8)
                | (cdpGravity1 << PHYS_GRAVITY_1)
                | (cdpGravity2 << PHYS_GRAVITY_2)
                | (cdpGravity3 << PHYS_GRAVITY_4)
                | (cdpGravity4 << PHYS_GRAVITY_8)
                | (lolRoulette1 << ITEM_ROULETTE_1)
                | (lolRoulette2 << ITEM_ROULETTE_2)
                | (lolRoulette3 << ITEM_ROULETTE_4)
                | (lolTTitembool << ITEM_START_ENABLED)
                | (lolTTitem1 << ITEM_START_1)
                | (lolTTitem2 << ITEM_START_2)
                | (lolTTitem3 << ITEM_START_4)
                | (lolTTitem4 << ITEM_START_8)
                | (lolTTitem5 << ITEM_START_16);
    }

    // WDD Contexts
    u32 contextWDD = 0;
    if(isCT) { //contexts that should only exist when CTs are on
        contextWDD = 0;
    }
    if(isLOL) { contextWDD //contexts that should only ever not exist for RTWWs
                |=(wddtceffect1 << ITEM_CLOUD_1)
                | (wddtceffect2 << ITEM_CLOUD_2)
                | (wddtceffect3 << ITEM_CLOUD_4)
                | (wddtceffect4 << ITEM_CLOUD_8);
    }
    this->contextPul = contextPul;
    this->contextLOL = contextLOL;
    this->contextWDD = contextWDD;

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
    if(self->IsContextPul(PULSAR_MODE_OTT)) OTT::AddGhostToVS();
    if(self->IsContextPul(PULSAR_HAW_1) && self->IsContextPul(PULSAR_MODE_KO) && GameScene::GetCurrent()->id == SCENE_ID_RACE && SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber > 0) {
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