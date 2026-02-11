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
    bool isCT = true;
    bool isLOL = true;
    bool isHAW = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) > 0;
    bool lolHAW = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) > 1;
    bool isKO = false;
    bool isOTT = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_VERSUS);
    bool isMiiHeads = settings.GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_MII);
    //lolpack divider
    bool lolLapType1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_RADIO_LAPSTYPE),1);
    bool lolLapType2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_RADIO_LAPSTYPE),2);
    bool lolLaps1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_LAPCOUNT),1);
    bool lolLaps2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_LAPCOUNT),2);
    bool lolLaps4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_LAPCOUNT),3);
    bool lolLaps8 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_LAPCOUNT),4);
    bool lolSpeeds1 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_SPEEDMOD),1);
    bool lolSpeeds2 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_SPEEDMOD),2);
    bool lolSpeeds4 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_SPEEDMOD),3);
    bool lolSpeeds8 = BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_SPEEDMOD),4);
    bool lolRoulette1 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_ROULETTE)+1)%6,1); // 6 settings
    bool lolRoulette2 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_ROULETTE)+1)%6,2); // important to define last setting as 0
    bool lolRoulette4 = BlFa3::getbin((settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_SCROLL_ROULETTE)+1)%6,3); // so add one and mod by highest setting value
    bool lolTTitem1 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEM),1);
    bool lolTTitem2 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEM),2);
    bool lolTTitem4 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEM),3);
    bool lolTTitemcount1 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEMCOUNT),1);
    bool lolTTitemcount2 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEMCOUNT),2);
    bool lolTTitemcount4 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEMCOUNT),3);
    bool lolTTitemcount8 = BlFa3::getbin(settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_TTITEMCOUNT),4);

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const GameMode mode = racedataSettings.gamemode;
    Network::Mgr& netMgr = this->netMgr;
    const u32 sceneId = GameScene::GetCurrent()->id;

    //bool is200 = racedataSettings.engineClass == CC_100 && this->info.Has200cc(); //unused
    bool isFeather = this->info.HasFeather();
    //bool isUMTs = this->info.HasUMTs(); // The normal definition for umts is undesirable now that we have trophy validation
    bool isUMTs = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ALLOWUMTS); // Instead take setting directly from tt settings to suit user preference
    bool isMegaTC = this->info.HasMegaTC();

    bool isLolBrake = 1; // At this stage, this value means "Is Brake Drifting Allowed?" since it needs to be calculated later

    u32 newContext = 0;
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
                lolLaps4 = 0;
                lolLaps8 = 0;
                // 1.0x Speed
                lolSpeeds1 = 0;
                lolSpeeds2 = 0;
                lolSpeeds4 = 0;
                lolSpeeds8 = 0;
                // Standard Roulette
                lolRoulette1 = 1;
                lolRoulette2 = 0;
                lolRoulette4 = 0;
                // For ott regionals
                // Mushroom
                lolTTitem1 = 0;
                lolTTitem2 = 0;
                lolTTitem4 = 0;
                // Default Count
                lolTTitemcount1 = 0;
                lolTTitemcount2 = 0;
                lolTTitemcount4 = 0;
                lolTTitemcount8 = 0;
                // Take UMTs from pack creator settings
                isUMTs = this->info.HasUMTs();
                break;
            case(RKNet::ROOMTYPE_JOINING_REGIONAL):
                isOTT = netMgr.ownStatusData == true;
                break;
            case(RKNet::ROOMTYPE_FROOM_HOST):
                break;
            case(RKNet::ROOMTYPE_FROOM_NONHOST):
                isCT = mode != MODE_BATTLE && mode != MODE_PUBLIC_BATTLE && mode != MODE_PRIVATE_BATTLE;
                newContext = netMgr.hostContext;
                isHAW = newContext & (1 << PULSAR_HAW);
                isKO = newContext & (1 << PULSAR_MODE_KO);
                isOTT = newContext & (1 << PULSAR_MODE_OTT);
                isMiiHeads = newContext & (1 << PULSAR_MIIHEADS);
                isUMTs = newContext & (1 << PULSAR_UMTS); // Usually defined alongside feathers for ott, but we don't want that
                //lol settings
                isLolBrake = newContext & (1 << LOLPACK_BRAKE); // Is Brake Drift Allowed? take host settings
                lolLapType1 = newContext & (1 << LOLPACK_LAPS_BIN1);
                lolLapType2 = newContext & (1 << LOLPACK_LAPS_BIN2);
                lolLaps1 = newContext & (1 << LOLPACK_LAPCOUNT_BIN1);
                lolLaps2 = newContext & (1 << LOLPACK_LAPCOUNT_BIN2);
                lolLaps4 = newContext & (1 << LOLPACK_LAPCOUNT_BIN4);
                lolLaps8 = newContext & (1 << LOLPACK_LAPCOUNT_BIN8);
                lolSpeeds1 = newContext & (1 << LOLPACK_SPEEDMOD_BIN1);
                lolSpeeds2 = newContext & (1 << LOLPACK_SPEEDMOD_BIN2);
                lolSpeeds4 = newContext & (1 << LOLPACK_SPEEDMOD_BIN4);
                lolSpeeds8 = newContext & (1 << LOLPACK_SPEEDMOD_BIN8);
                lolRoulette1 = newContext & (1 << LOLPACK_ROULETTE_BIN1);
                lolRoulette2 = newContext & (1 << LOLPACK_ROULETTE_BIN2);
                lolRoulette4 = newContext & (1 << LOLPACK_ROULETTE_BIN4);
                lolTTitem1 = newContext & (1 << LOLPACK_TTITEM_BIN1);
                lolTTitem2 = newContext & (1 << LOLPACK_TTITEM_BIN2);
                lolTTitem4 = newContext & (1 << LOLPACK_TTITEM_BIN4);
                lolTTitemcount1 = newContext & (1 << LOLPACK_TTITEMCOUNT_BIN1);
                lolTTitemcount2 = newContext & (1 << LOLPACK_TTITEMCOUNT_BIN2);
                lolTTitemcount4 = newContext & (1 << LOLPACK_TTITEMCOUNT_BIN4);
                lolTTitemcount8 = newContext & (1 << LOLPACK_TTITEMCOUNT_BIN8);
                //end of lol settings
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
    this->netMgr.hostContext = newContext;

    // Brake Drift needs to be calculated after room settings since it differs based on several settings
    isLolBrake &= (BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_RADIO_BRAKE),2)| // If setting = 1X, always enable
                  (!BlFa3::getbin(settings.GetUserSettingValue(Settings::SETTINGSTYPE_GAME, SETTINGGAME_RADIO_BRAKE),1) // If setting = 01, disable. Else, check speedmod.
                  & (lolSpeeds1+lolSpeeds2*2+lolSpeeds4*4+lolSpeeds8*8 > 1) // Lower bound 2 = 1.25x speed
                  & (lolSpeeds1+lolSpeeds2*2+lolSpeeds4*4+lolSpeeds8*8 < 9) // Upper bound 8 = 100x speed
                  )); // AND assignment since we need to know if brakedrifting is allowed

    // Validate Time Trials
    bool isValidTT = ! // NOT gate result
                     //Valid lapcount would be 0000, = 3. (It'd be better to check directly with kmp, but not possible for most menus.)
                     (lolLaps1 |lolLaps2 |lolLaps4 |lolLaps8
                     //Calculated and Exclusive laps are always correct for 3 laps, but Forced laps cause issues, so restrict it.
                     |lolLapType2
                     //if speed is high, ignore as this is enabled anyway.
                     |(isLolBrake
                         ^((lolSpeeds1+lolSpeeds2*2+lolSpeeds4*4+lolSpeeds8*8 > 1) // Lower bound 2 = 1.25x speed
                         & (lolSpeeds1+lolSpeeds2*2+lolSpeeds4*4+lolSpeeds8*8 < 9) // Upper bound 8 = 100x speed
                     ))
                     // Verify UMT setting matches pack setting
                     |(this->info.HasUMTs() != isUMTs)
                     // We run into a slight problem here - TT items depend on the mode, selected via menu buttons
                     // Eventual solution - Condense all buttons into settings
                     // Temporary solution - Make all buttons do the same thing lmao
                     //Valid item counts are weird, though as long as only mushrooms and feathers are normal categories we can be lenient (set to only default for now tho)
                     |lolTTitemcount1 |lolTTitemcount2 |lolTTitemcount4 |lolTTitemcount8
                     );

// Extra validity notes:
// - All speedmods are valid now since they're split up into categories, so we don't need to check for that
// - All items are also valid for the same reason
// - Blooper TTs are incorrectly read as valid Feather TTs. No fix?



    u32 context = (isCT << PULSAR_CT)
                | (isHAW << PULSAR_HAW)
                | (lolHAW << LOLPACK_HAWTYPE)
                | (isMiiHeads << PULSAR_MIIHEADS)
                //lolpack divider
                | (isValidTT << LOLPACK_VALID_TTS);
    if(isCT) { //contexts that should only exist when CTs are on
        context |=(isOTT << PULSAR_MODE_OTT)
                //|(is200 << PULSAR_200) // Unused
                //lolpack divider
                | (lolLapType1 << LOLPACK_LAPS_BIN1)
                | (lolLapType2 << LOLPACK_LAPS_BIN2)
                | (lolLaps1 << LOLPACK_LAPCOUNT_BIN1)
                | (lolLaps2 << LOLPACK_LAPCOUNT_BIN2)
                | (lolLaps4 << LOLPACK_LAPCOUNT_BIN4)
                | (lolLaps8 << LOLPACK_LAPCOUNT_BIN8)
                | (lolTTitem1 << LOLPACK_TTITEM_BIN1)
                | (lolTTitem2 << LOLPACK_TTITEM_BIN2)
                | (lolTTitem4 << LOLPACK_TTITEM_BIN4)
                | (lolTTitemcount1 << LOLPACK_TTITEMCOUNT_BIN1)
                | (lolTTitemcount2 << LOLPACK_TTITEMCOUNT_BIN2)
                | (lolTTitemcount4 << LOLPACK_TTITEMCOUNT_BIN4)
                | (lolTTitemcount8 << LOLPACK_TTITEMCOUNT_BIN8);
    }
    if(isLOL) { //contexts that should only ever not exist for RTWWs
        context |=(isFeather << PULSAR_FEATHER)
                | (isUMTs << PULSAR_UMTS)
                | (isMegaTC << PULSAR_MEGATC)
                | (isKO << PULSAR_MODE_KO)
                //lolpack divider
                | (isLolBrake << LOLPACK_BRAKE)
                | (lolSpeeds1 << LOLPACK_SPEEDMOD_BIN1)
                | (lolSpeeds2 << LOLPACK_SPEEDMOD_BIN2)
                | (lolSpeeds4 << LOLPACK_SPEEDMOD_BIN4)
                | (lolSpeeds8 << LOLPACK_SPEEDMOD_BIN8)
                | (lolRoulette1 << LOLPACK_ROULETTE_BIN1)
                | (lolRoulette2 << LOLPACK_ROULETTE_BIN2)
                | (lolRoulette4 << LOLPACK_ROULETTE_BIN4);
    }
    this->context = context;

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
    if(self->IsContext(PULSAR_MODE_OTT)) OTT::AddGhostToVS();
    if(self->IsContext(PULSAR_HAW) && self->IsContext(PULSAR_MODE_KO) && GameScene::GetCurrent()->id == SCENE_ID_RACE && SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber > 0) {
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