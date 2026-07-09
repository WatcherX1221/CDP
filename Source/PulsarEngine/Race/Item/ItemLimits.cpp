#include <PulsarSystem.hpp>
#include <kamek.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>



namespace Pulsar {
namespace Race {

// Item Limits
// Credits to Brawlbox for VP Item Limit loading

enum itemLimitDef {
    ITEMLIMIT_VANILLA = 0x0,
    ITEMLIMIT_STANDARD = 0x1,
    ITEMLIMIT_LARGE = 0x2,
    ITEMLIMIT_SMALL = 0x3 // debug option
};

static const u8 itemLimits[4][17] = { // Table of item limits
//  GRNS REDS BANA MUSH STAR BLUE SHOK FIBX MEGA BOMB BLPR POWB GMUS BILL CLOU NONE FEAT
    { 12,   8,  16,  12,   3,   1,   1,   6,   2,   3,   1,   1,   2,   1,   1,   0,   0 }, // Vanilla
    { 12,   8,  12,  12,   4,   3,   2,  12,   4,   8,   2,   2,   3,   2,   3,   0,   4 }, // Standard
    { 16,  16,  32,  16,  12,  16,   8,  32,  12,  32,   6,   6,   8,   8,  12,   0,   8 }, // Large Item Counts
    {  3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3 }  // Small
};

static const u8 itemSettingLimit[5] = {
    ITEMLIMIT_STANDARD, // Standard
    ITEMLIMIT_STANDARD, // Mushrooms
    ITEMLIMIT_STANDARD, // Random
    ITEMLIMIT_LARGE, // Debalanced
    ITEMLIMIT_VANILLA // Vanilla
};

static void limitGreenShell(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_GREEN_SHELL];
}
static void limitRedShell(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_RED_SHELL];
}
static void limitBanana(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_BANANA];
}
static void limitMushroom(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_MUSHROOM];
}
static void limitStar(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_STAR];
}
static void limitBlue(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_BLUE_SHELL];
}
static void limitLightning(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_LIGHTNING];
}
static void limitFakeItemBox(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_FAKE_ITEM_BOX];
}
static void limitMega(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_MEGA_MUSHROOM];
}
static void limitBomb(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_BOBOMB];
}
static void limitBlooper(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_BLOOPER];
}
static void limitPOW(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_POW_BLOCK];
}
static void limitGolden(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_GOLDEN_MUSHROOM];
}
static void limitBill(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_BULLET_BILL];
}
static void limitCloud(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    new (dest) Item::ObjProperties(rel);
    const u8 itemLimiter = itemSettingLimit[System::sInstance->GetContext(ITEM_ROULETTEBIN)];
    dest->limit = itemLimits[itemLimiter][OBJ_THUNDER_CLOUD];
}

kmCall(0x80790b20, limitGreenShell);
kmCall(0x80790b34, limitRedShell);
kmCall(0x80790b44, limitBanana);
kmCall(0x80790b54, limitMushroom);
kmCall(0x80790b64, limitStar);
kmCall(0x80790b74, limitBlue);
kmCall(0x80790b84, limitLightning);
kmCall(0x80790b94, limitFakeItemBox);
kmCall(0x80790ba4, limitMega);
kmCall(0x80790bb4, limitBomb);
kmCall(0x80790bc4, limitBlooper);
kmCall(0x80790bd4, limitPOW);
kmCall(0x80790be4, limitGolden);
kmCall(0x80790bf4, limitBill);
kmCall(0x80790c04, limitCloud);

} // namespace Race
} // namespace Pulsar