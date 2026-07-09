#include <kamek.hpp>
#include <MarioKartWii/Kart/KartCollision.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <PulsarSystem.hpp>
#include <Settings/SettingsParam.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
//#include <MarioKartWii/Item/Obj/Bomb.hpp>


namespace Pulsar {
namespace Race {

// Credits to Brawlbox for All Items Can Land

// Pickup Properties

static int UseItem(Kart::Collision *kartCollision, ItemId id) {
    u8 playerId = kartCollision->GetPlayerIdx();
    Item::Manager::sInstance->players[playerId].inventory.currentItemCount++;
    Item::Behavior::behaviourTable[id].useFunction(Item::Manager::sInstance->players[playerId]);
    return -1;
}
static int PickupItem(Kart::Collision *kartCollision, ItemId item, ItemId backup) {
    u8 playerId = kartCollision->GetPlayerIdx();
    if( Item::Manager::sInstance->players[playerId].inventory.currentItemId == ITEM_NONE ) {
        Item::Manager::sInstance->players[playerId].inventory.currentItemId = item;
        Item::Manager::sInstance->players[playerId].inventory.currentItemCount = 1;
        if (item == GOLDEN_MUSHROOM) {
            Item::Manager::sInstance->players[playerId].inventory.hasGolden = true;
            Item::Manager::sInstance->players[playerId].inventory.goldenTimer = 240;
        }
    }
    else if( backup != ITEM_NONE ) return UseItem(kartCollision, backup);
    return -1;
}

// Ground Collision Definitions

static int GroundCollisionLightning(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return UseItem(kartCollision, LIGHTNING);
    return -1;
}
static int GroundCollisionMega(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return UseItem(kartCollision, MEGA_MUSHROOM);
    return -1;
}
static int GroundCollisionBlooper(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return UseItem(kartCollision, BLOOPER);
    return -1;
}
static int GroundCollisionPOW(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return UseItem(kartCollision, POW_BLOCK);
    return -1;
}
static int GroundCollisionGolden(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return PickupItem(kartCollision, GOLDEN_MUSHROOM, MUSHROOM);
    return -1;
}
static int GroundCollisionBill(Kart::Collision *kartCollision){
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) return UseItem(kartCollision, BULLET_BILL);
    return -1;
}

//kmWritePointer(0x808b5470, GroundCollisionGreenShell);
//kmWritePointer(0x808b547c, GroundCollisionRedShell);
//kmWritePointer(0x808b5488, GroundCollisionBanana);
//kmWritePointer(0x808b5494, GroundCollisionMushroom);
//kmWritePointer(0x808b54a0, GroundCollisionStar);
//kmWritePointer(0x808b54ac, GroundCollisionBlue);
kmWritePointer(0x808b54b8, GroundCollisionLightning);
//kmWritePointer(0x808b54c4, GroundCollisionFakeItemBox);
kmWritePointer(0x808b54d0, GroundCollisionMega);
//kmWritePointer(0x808b54dc, GroundCollisionBomb);
kmWritePointer(0x808b54e8, GroundCollisionBlooper);
kmWritePointer(0x808b54f4, GroundCollisionPOW);
kmWritePointer(0x808b5500, GroundCollisionGolden);
kmWritePointer(0x808b550c, GroundCollisionBill);
//kmWritePointer(0x808b5518, GroundCollisionCloud);

// Credits to Retro Rewind for landable items modifying

// Item Landing Definitions

void AllowDroppedItems() {
    if ( System::sInstance->GetContext(ITEM_ROULETTEBIN) != ITEMSETTING_ROULETTE_VANILLA ) {
        for (int i = 0; i < 15; i++) {
            Item::ObjProperties::objProperties[i].canFallOnTheGround = true;
        }
    }
}
kmBranch(0x80790af8, AllowDroppedItems);

// Credits to VP for bob-omb timer modifications

// Bob-omb timer modifications

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

} // Namespace Race
} // Namespace Pulsar