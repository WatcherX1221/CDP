#ifndef _CDP_ITEM_
#define _CDP_ITEM_
#include <kamek.hpp>

namespace CIRNO {

struct CDPIHeader {
    static const char[4] goodMagic = 'CDPI';
    char[4] magic;
    u32 offsetItemSlot;
    u32 offsetItemLand;
    u32 offsetSettings;
};

struct CDPIItemSlot {
    u8 itemSlots;
};

struct CDPIItemLand {
    u8 landIndexes;
    struct Index {
        u8 items;
        struct Properties {
            u8 limit;
            u8 propertyBitfield;
            u8 function;
            u16 cooldownGo;
            u16 cooldownGet;
            u16 timerGolden;
            u8 extra;
        };
        Properties itemProperties[items];
    };
    Index itemLand[landIndexes];
};

struct CDPISettings {
    u8 settings;
};

}//namespace CIRNO
#endif