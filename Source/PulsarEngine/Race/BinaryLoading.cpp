#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace Race {

// Credits to Brawlbox for Variety Pack common binary loading

// Custom Roulette odds loading
static void *GetCustomItemSlot(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    switch ( System::sInstance->GetContext(ITEM_ROULETTEBIN) ) {
        case ITEMSETTING_ROULETTE_STANDARD:   name = "ItemSlotStandard.bin"; break;
        case ITEMSETTING_ROULETTE_MUSHROOMS:  name = "ItemSlotMushroom.bin"; break;
        case ITEMSETTING_ROULETTE_RANDOM:     name = "ItemSlotRandom.bin"; break;
        case ITEMSETTING_ROULETTE_DEBALANCED: name = "ItemSlotDebalanced.bin"; break;
        case ITEMSETTING_ROULETTE_THUNDERSTORM: name = "ItemSlotThunderstorm.bin"; break;
    default: name = "ItemSlot.bin";
    }
    return archive->GetFile(type, name, length);
}

kmCall(0x807bb128, GetCustomItemSlot);
kmCall(0x807bb030, GetCustomItemSlot);
kmCall(0x807bb200, GetCustomItemSlot);
kmCall(0x807bb53c, GetCustomItemSlot);
kmCall(0x807bbb58, GetCustomItemSlot);
kmCall(0x807bbdd4, GetCustomItemSlot);
kmCall(0x807bbf50, GetCustomItemSlot);

// Custom Vehicle Stat Loading
static void *GetCustomKartParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    switch ( System::sInstance->GetContext(PHYS_KARTBIN) ) {
        case PHYSSETTING_KARTSTAT_GOLD: name = "kartParamGold.bin"; break;
        case PHYSSETTING_KARTSTAT_PLATINUM: name = "kartParamPlatinum.bin"; break;
        case PHYSSETTING_KARTSTAT_DIAMOND: name = "kartParamDiamond.bin"; break;
        case PHYSSETTING_KARTSTAT_DRIFTSWAP: name = "kartParamDriftSwap.bin"; break;
        case PHYSSETTING_KARTSTAT_BOLLOCKS: name = "kartParamBollocks.bin"; break;
        default: name = "kartParam.bin";
    }
    return archive->GetFile(type, name, length);
}
kmCall(0x80591a30, GetCustomKartParam);

// Custom Driver Stat Loading
static void *GetCustomDriverParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    switch ( System::sInstance->GetContext(PHYS_KARTBIN) ) {
        case PHYSSETTING_KARTSTAT_PLATINUM: name = "driverParamPlatinum.bin"; break;
        default: name = "driverParamMiiFix.bin";
    }
    return archive->GetFile(type, name, length);
}
//kmCall(0x80591a54, GetCustomDriverParam); // Unsure what the proper address for this is. Try this one?
// Use Intended Mii Stats [B_squo]
//kmWrite32(0x80592163, 0x00000018);
//kmWrite32(0x80592143, 0x000000E8);

} // namespace Race
} // namespace Pulsar
