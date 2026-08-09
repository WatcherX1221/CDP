#include <PulsarSystem.hpp>
#include <kamek.hpp>
#include <MarioKartWii/Item/ItemSlot.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <Settings/SettingsParam.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

// Credits to Brawlbox for Accurate Roulette

namespace Pulsar {
namespace Race{

// TEMP
//kmWrite32(0x807965C0, 0x60000000); // No Item Poof
//kmWrite32(0x8065F270, 0x60000000); // Disable No Item Available Fallback
//kmWrite32(0x8065F630, 0x60000000);
// TEMP

// Accurate roulettes for supported modes
static int AccurateItemRoulette(Item::ItemSlotData *itemSlotData, u16 itemBoxType, u8 position, ItemId prevRandomItem, bool r7){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) {
        const u8 playerId = Raceinfo::sInstance->playerIdInEachPosition[position-1];
        const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
        if (gameMode != MODE_BATTLE &&
        gameMode != MODE_PRIVATE_BATTLE && 
        gameMode != MODE_PUBLIC_BATTLE){
            Item::Player *itemPlayer = &Item::Manager::sInstance->players[playerId];
            return itemSlotData->DecideItem(itemBoxType, position, itemPlayer->isHuman, 0x1, itemPlayer);
        }
    }
    return itemSlotData->DecideRouletteItem(itemBoxType, position, prevRandomItem, r7);
}

// Accurate Roulette
kmCall(0x807ba1e4, AccurateItemRoulette);
kmCall(0x807ba428, AccurateItemRoulette);
kmCall(0x807ba598, AccurateItemRoulette);

// Patch Mushroom Bug
// Mushroom Glitch Fix [Vabold]
static Item::PlayerRoulette* ApplyMushroomGlitchFix(Item::PlayerRoulette* roulette) {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    if (controller != nullptr && (System::sInstance->GetContext(ITEM_ROULETTEBIN) == ITEMSETTING_ROULETTE_VANILLA)) ++roulette->itemNum;
    return roulette;
}
kmCall(0x807BA078, ApplyMushroomGlitchFix);

// Extend bobomb timer
// Credits to Brawlbox for VP Bobomb timer extensions

/*
static void DroppedBombTimer(Item::ObjBomb* obj){
    u32 timer = 300;
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) == ITEMSETTING_ROULETTE_DEBALANCED ){
        timer = 4095;
    }
    obj->timer = timer;
    obj->Item::Obj::SpawnModel();
}
kmCall(0x807a5be4, DroppedBombTimer);
kmWrite32(0x807a5c10, 0x60000000); // nop the store of the timer

static void ThrownBombTimer(Item::ObjBomb* obj, Item::PlayerSub& playerSub, u32 groundEffectDelay, bool isThrow, float speed, float throwHeight, float dropHeight){
    obj->SetInitialPositionImpl(playerSub, groundEffectDelay, isThrow, speed, throwHeight, dropHeight);
    u32 timer = 90;
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) == ITEMSETTING_ROULETTE_DEBALANCED ){
        timer = 4095;
    }
    obj->timer = timer;
}
kmCall(0x807a4ac4, ThrownBombTimer);
kmWrite32(0x807a4acc, 0x60000000); // nop the store of the timer
*/

// also want to do 047965C0 60000000 Disable Item Poof [CLF78]

} // namespace Race
} // namespace Pulsar
