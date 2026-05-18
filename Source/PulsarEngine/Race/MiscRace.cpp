#include <kamek.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceBalloon.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceResult.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace Race {

static void NonGhostPlayerCount(RacedataScenario& scenario, u8* playerCount, u8* screenCount, u8* localPlayerCount) {
    scenario.ComputePlayerCounts(playerCount, screenCount, localPlayerCount);
    System* system = System::sInstance;
    u8 realPlayers = *playerCount;
    if (scenario.settings.gamemode != MODE_TIME_TRIAL) for (int i = 0; i < 12; ++i) if (scenario.players[i].playerType == PLAYER_GHOST) --realPlayers;
    system->nonTTGhostPlayersCount = realPlayers;
}
kmCall(0x8052fc78, NonGhostPlayerCount);

kmWrite32(0x807997e0, 0x60000000);
//Starting item for OTT and TT, id is TRIPLE_MUSHROOM by default
//Edited to support lolpack settings
static void SetStartingItem(Item::PlayerInventory& inventory, ItemId id, bool isItemForcedDueToCapacity) {
    register u32 playerId;
    asm(mr playerId, r29;);
    if (Racedata::sInstance->racesScenario.players[playerId].playerType == PLAYER_CPU) return;
    const System* system = System::sInstance;
    const bool isTT = DriverMgr::isTT;
    if ((isTT || system->IsContextPul(PULSAR_MODE_OTT)) || system->IsContextLOL(ITEM_START_ENABLED)) {
    bool isFeather;
    if (isTT) { //Should probably remove this but right now I don't care much
        const TTMode mode = system->ttMode;
        isFeather = (mode == TTMODE_150_FEATHER || mode == TTMODE_200_FEATHER);
    }
    else isFeather = system->IsContextPul(PULSAR_FEATHER);
    // Switch for item
    if ( system->IsContextLOL(ITEM_START_ENABLED) ) {
    switch(system->IsContextLOL(ITEM_START_1)
          +system->IsContextLOL(ITEM_START_2)*2
          +system->IsContextLOL(ITEM_START_4)*4
          +system->IsContextLOL(ITEM_START_8)*8
          +system->IsContextLOL(ITEM_START_16)*16
          ){
    case ITEMSETTING_START_MUSH:
        id = MUSHROOM;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3MUS:
        id = TRIPLE_MUSHROOM;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_GMUS:
        id = GOLDEN_MUSHROOM;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_FEAT:
        id = FEATHER;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3FEA:
        id = TRIPLE_FEATHER;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_STAR:
        id = STAR;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_MEGA:
        id = MEGA_MUSHROOM;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_CLOU:
        id = THUNDER_CLOUD;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_GREN:
        id = GREEN_SHELL;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3GRN:
        id = TRIPLE_GREEN_SHELL;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_REDS:
        id = RED_SHELL;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3RED:
        id = TRIPLE_RED_SHELL;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_BLUE:
        id = BLUE_SHELL;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_BANA:
        id = BANANA;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3BNA:
        id = TRIPLE_BANANA;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_FIBX:
        id = FAKE_ITEM_BOX;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_3FIB:
        id = TRIPLE_FAKE_ITEM_BOX;
        inventory.currentItemCount = 3;
        break;
    case ITEMSETTING_START_BOMB:
        id = BOBOMB;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_BLPR:
        id = BLOOPER;
        inventory.currentItemCount = 1;
        break;
//    case ITEMSETTING_START_GHST:
//        id = BOO;
//        inventory.currentItemCount = 1;
//        break;
    case ITEMSETTING_START_POWB:
        id = POW_BLOCK;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_BILL:
        id = BULLET_BILL;
        inventory.currentItemCount = 1;
        break;
    case ITEMSETTING_START_SHOK:
        id = LIGHTNING;
        inventory.currentItemCount = 1;
        break;
    default:
        id = ITEM_NONE;
    }}
    else {
        id = TRIPLE_MUSHROOM;
        inventory.currentItemCount = 3;
        }
    if (id != ITEM_NONE) inventory.SetItem(id, isItemForcedDueToCapacity);
    }
}
kmCall(0x80799808, SetStartingItem);

//From JoshuaMK, ported to C++ by Brawlbox and adapted as a setting
static int MiiHeads(Racedata* racedata, u32 unused, u32 unused2, u8 id) {
    CharacterId charId = racedata->racesScenario.players[id].characterId;
    if (System::sInstance->IsContextPul(PULSAR_MIIHEADS)) {
        if (charId < MII_M) {
            if (id == 0) charId = MII_M;
            else if (RKNet::Controller::sInstance->connectionState != 0) charId = MII_M;
        }
    }
    return charId;
}
kmCall(0x807eb154, MiiHeads);
kmWrite32(0x807eb15c, 0x60000000);
kmWrite32(0x807eb160, 0x88de01b4);

//credit to XeR for finding the float address
static void BattleGlitchEnable() {
    const u8 val = Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_BATTLE);
    float maxDistance = 7500.0f;
    if (val == RACESETTING_BATTLE_GLITCH_ENABLED) maxDistance = 75000.0f;
    System* system = System::sInstance;
    if (system->IsContextPul(PULSAR_MODE_OTT)) {
        const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[0].controllerHolder;
        const ControllerType controllerType = controllerHolder->curController->GetType();
        const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
        const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);
        u32 toggleInput = PAD::PAD_BUTTON_Y;
        switch (controllerType) {
        case NUNCHUCK:
            toggleInput = WPAD::WPAD_BUTTON_DOWN;
            break;
        case WHEEL:
            toggleInput = WPAD::WPAD_BUTTON_MINUS;
            break;
        case CLASSIC:
            toggleInput = WPAD::WPAD_CL_TRIGGER_ZL;
            break;
        }
        if ((newInputs & toggleInput) == toggleInput) system->ottHideNames = !system->ottHideNames;
        if (system->ottHideNames) maxDistance -= maxDistance;
    }
    RaceBalloons::maxDistanceNames = maxDistance;
}
RaceFrameHook BattleGlitch(BattleGlitchEnable);

kmWrite32(0x8085C914, 0x38000000); //times at the end of races in VS
static void DisplayTimesInsteadOfNames(CtrlRaceResult& result, u8 id) {
    result.FillFinishTime(id);
}
kmCall(0x8085d460, DisplayTimesInsteadOfNames); //for WWs

//don't hide position tracker (MrBean35000vr)
kmWrite32(0x807F4DB8, 0x38000001);

//Draggable blue shells
static void DraggableBlueShells(Item::PlayerObj& sub) {
    if (Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_BLUES) == RACESETTING_DRAGGABLE_BLUES_DISABLED) {
        sub.isNotDragged = true;
    }
}
kmBranch(0x807ae8ac, DraggableBlueShells);

//Coloured Minimap
kmWrite32(0x807DFC24, 0x60000000);

//No Team Invincibility
kmWrite32(0x8056fd24, 0x38000000); //KartCollision::CheckKartCollision()
kmWrite32(0x80572618, 0x38000000); //KartCollision::CheckItemCollision()
kmWrite32(0x80573290, 0x38000000); //KartCollision::HandleFIBCollision()
kmWrite32(0x8068e2d0, 0x38000000); //PlayerEffects ctor
kmWrite32(0x8068e314, 0x38000000); //PlayerEffects ctor
//kmWrite32(0x807a7f6c, 0x38c00000); //FIB are always red //funny
kmWrite32(0x807b0bd4, 0x38000000); //pass TC to teammate
kmWrite32(0x807bd2bc, 0x38000000); //RaceGlobals
kmWrite32(0x807f18c8, 0x38000000); //TC alert

//Accurate Explosion Damage (MrBean, CLF)
kmWrite16(0x80572690, 0x4800);
kmWrite16(0x80569F68, 0x4800);


//CtrlItemWindow
kmWrite24(0x808A9C16, 'PUL'); //item_window_new -> item_window_PUL

const char* ChangeItemWindowPane(ItemId id, u32 itemCount) {
    const bool feather = System::sInstance->IsContextPul(PULSAR_FEATHER);
    const bool megaTC = System::sInstance->IsContextPul(PULSAR_MEGATC);
    const char* paneName;
    switch (id) {
        case THUNDER_CLOUD:
            switch
            ( System::sInstance->IsContextWDD(ITEM_CLOUD_1)
            + System::sInstance->IsContextWDD(ITEM_CLOUD_2)*2
            + System::sInstance->IsContextWDD(ITEM_CLOUD_4)*4
            + System::sInstance->IsContextWDD(ITEM_CLOUD_8)*8
            ) {
            case ITEMSETTING_CLOUD_SHOCK: GetItemIconPaneName(id, itemCount); //Shock;
            case ITEMSETTING_CLOUD_MEGA: paneName = "megaTC" ;break; // Mega
            case ITEMSETTING_CLOUD_STAR: paneName = "starTC" ;break; // Star
            //case ITEMSETTING_CLOUD_FEATHER: paneName = "featherTC" ;break; // Feather
            //case ITEMSETTING_CLOUD_DEATH: paneName = "deathTC" ;break; // Death
            case ITEMSETTING_CLOUD_BLOOPER: paneName = "blooperTC" ;break; // Blooper
            //case ITEMSETTING_CLOUD_MEGASTAR: paneName = "megastarTC" ;break; // Mega Star
            //case ITEMSETTING_CLOUD_SHOCKBLOOPER: paneName = "shockblooperTC" ;break; // Shock Blooper
            //case ITEMSETTING_CLOUD_SHOCKMEGA: paneName = "dizzyTC" ;break; // Dizzy
            //case ITEMSETTING_CLOUD_MEGASHOCK: paneName = "cloudTC" ;break; // Cloud
            default: paneName = "wanwan" // Placeholder // Char wanted funny wanwan pane lol
            ;}break;
        case FEATHER: paneName = "feather" ;break;
        case TRIPLE_FEATHER: paneName = itemCount == 1 ? "feather" : itemCount == 2 ? "feather_2" : "feather_3" ;break;
        case TRIPLE_FAKE_ITEM_BOX: paneName = "dummybox_3" ;break;
        default: paneName = GetItemIconPaneName(id, itemCount);}
    return paneName;
}
kmCall(0x807f3648, ChangeItemWindowPane);
kmCall(0x807ef168, ChangeItemWindowPane);
kmCall(0x807ef3e0, ChangeItemWindowPane);
kmCall(0x807ef444, ChangeItemWindowPane);

kmWrite24(0x808A9FF3, 'PUL');
}//namespace Race
}//namespace Pulsar