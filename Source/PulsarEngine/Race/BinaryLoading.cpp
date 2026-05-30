#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace Race {

// Credits to Brawlbox for Variety Pack common binary loading

// Custom Roulette odds loading
static void *GetCustomItemSlot(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    switch ( System::sInstance->IsContextLOL(ITEM_ROULETTE_4)*4
           + System::sInstance->IsContextLOL(ITEM_ROULETTE_2)*2
           + System::sInstance->IsContextLOL(ITEM_ROULETTE_1)-1) { // Subtract 1 to maintain parity with zero vanilla
        case ITEMSETTING_ROULETTE_STANDARD:   name = "ItemSlotStandard.bin"; break;
        case ITEMSETTING_ROULETTE_MUSHROOMS:  name = "ItemSlotMushroom.bin"; break;
        case ITEMSETTING_ROULETTE_RANDOM:     name = "ItemSlotRandom.bin"; break;
        case ITEMSETTING_ROULETTE_DEBALANCED: name = "ItemSlotDebalanced.bin"; break;
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
    switch (System::sInstance->IsContextWDD(PHYS_KARTSTAT_1)) {
        case PHYSSETTING_KARTSTAT_PLATINUM: name = "kartParamPlatinum.bin"; break;
        default: name = "kartParam.bin";
    }
    return archive->GetFile(type, name, length);
}
kmCall(0x80591a30, GetCustomKartParam);

} // namespace Race
} // namespace Pulsar
