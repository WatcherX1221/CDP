#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <PulsarSystem.hpp>
#include <kamek.hpp>
#include <MarioKartWii/Item/ItemSlot.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <MarioKartWii/Kart/KartCollision.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>

// Credits to Brawlbox for Variety Pack common binary loading
// Credits to Brawlbox for Accurate Roulette
// Credits to Vega for Remove Mushroom Bug
// Credits to Brawlbox for All Items Can Land

namespace Pulsar {
namespace Race{

// Custom Roulette odds loading
static void *GetCustomItemSlot(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    switch( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
          + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
          + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) {
    case(0x1): // Standard
        name = "ItemSlot.bin";
        break;
    case(0x2): // Mushrooms
        name = "ItemSlotMushroom.bin";
        break;
    case(0x3): // Random
        name = "ItemSlotRandom.bin";
        break;
    case(0x4): // Debalanced
        name = "ItemSlotDebalanced.bin";
        break;
    case(0x5): // Overpowered
        name = "ItemSlotOverpowered.bin";
        break;
    default:   // Vanilla
        name = "ItemSlot.bin";
    }
    return archive->GetFile(type, name, length);
}

// Accurate roulettes for supported modes
static int AccurateItemRoulette(Item::ItemSlotData *itemSlotData, u16 itemBoxType, u8 position, ItemId prevRandomItem, bool r7){
    if (    ( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
            + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
            + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)
            ) > 0 ) {
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

static int PatchMushroomBug() { // Needs testing
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return 0x0;
    return -1;
}

// All Items Can Land
static int UseItem(Kart::Collision *kartCollision, ItemId id){
    u8 playerId = kartCollision->GetPlayerIdx();
    Item::Manager::sInstance->players[playerId].inventory.currentItemCount++;
    Item::Behavior::behaviourTable[id].useFunction(Item::Manager::sInstance->players[playerId]);
    return -1;
}
static int GroundCollisionShock(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, LIGHTNING);
    return -1;
}
static int GroundCollisionMega(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, MEGA_MUSHROOM);
    return -1;
}
static int GroundCollisionFeather(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, BLOOPER);
    return -1;
}
static int GroundCollisionPOW(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, POW_BLOCK);
    return -1;
}
static int GroundCollisionGolden(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, MUSHROOM);
    return -1;
}
static int GroundCollisionBullet(Kart::Collision *kartCollision){
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return UseItem(kartCollision, BULLET_BILL);
    return -1;
}
static int ItemLanding() {
    if (( System::sInstance->IsContext(LOLPACK_ROULETTE_BIN4)*4
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN2)*2
        + System::sInstance->IsContext(LOLPACK_ROULETTE_BIN1)) > 0 ) return 0x39800001;
    return 0x89840058;
}

// Item Slot
kmCall(0x807bb128, GetCustomItemSlot);
kmCall(0x807bb030, GetCustomItemSlot);
kmCall(0x807bb200, GetCustomItemSlot);
kmCall(0x807bb53c, GetCustomItemSlot);
kmCall(0x807bbb58, GetCustomItemSlot);
// Accurate Roulette
kmCall(0x807ba1e4, AccurateItemRoulette);
kmCall(0x807ba428, AccurateItemRoulette);
kmCall(0x807ba598, AccurateItemRoulette);
// Mushroom Bug
kmWritePointer(0x807BA077, PatchMushroomBug);
// All Items Can Land
kmWritePointer(0x808b54b8, GroundCollisionShock);
kmWritePointer(0x808b54d0, GroundCollisionMega);
kmWritePointer(0x808b54f4, GroundCollisionPOW);
kmWritePointer(0x808b5500, GroundCollisionGolden);
kmWritePointer(0x808b550c, GroundCollisionBullet);
//kmWritePointer(0x808b54e8, GroundCollisionFeather); // Already done in Pulsar
kmWrite32(0x807A66C4, 0x60000000);
kmWrite32(0x80796D30, 0x38600000);
kmWrite32(0x80790EF0, 0x89840058); // Needs fixing. Item Landing is disabled entirely until fix is devised
kmWrite32(0x80790EF4, 0x39600001);
kmWrite32(0x80790EF8, 0x39400001);
kmWrite32(0x80790EFC, 0x39200001);

} // namespace Race
} // namespace Pulsar
