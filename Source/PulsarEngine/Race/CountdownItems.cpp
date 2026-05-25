#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/System/Identifiers.hpp>

// Expanded behaviourTable in mod BSS (from ItemSlotExpansion.cpp)
extern "C" Item::Behavior expandedBehaviourTable[26];

namespace Pulsar {
namespace Race {


void FusionMushroomBoost(Item::Player& itemPlayer) {
    Kart::Movement* movement = itemPlayer.pointers->kartMovement;
    if (movement != nullptr) {
        movement->ActivateMushroom();
    }
}


static void RegisterCtdnItemBehaviours() {

    Item::Behavior& greenShellMush = expandedBehaviourTable[GREEN_SHELL_MUSHROOM];
    greenShellMush.unknkown_0x0 = 1;
    greenShellMush.unknkown_0x1 = 1;
    greenShellMush.objId = OBJ_GREEN_SHELL;
    greenShellMush.numberOfItems = 1;
    greenShellMush.unknown_0xc = 0;
    greenShellMush.unknown_0x10 = 0;
    greenShellMush.useType = Item::ITEMUSE_FIRE;
    greenShellMush.useFunction = FusionMushroomBoost;

    Item::Behavior& bobombMush = expandedBehaviourTable[BOBOMB_MUSHROOM];
    bobombMush.unknkown_0x0 = 1;
    bobombMush.unknkown_0x1 = 1;
    bobombMush.objId = OBJ_BOBOMB;
    bobombMush.numberOfItems = 1;
    bobombMush.unknown_0xc = 0;
    bobombMush.unknown_0x10 = 0;
    bobombMush.useType = Item::ITEMUSE_FIRE;
    bobombMush.useFunction = FusionMushroomBoost;
}

RaceLoadHook RegisterCtdnItems(RegisterCtdnItemBehaviours);

} // namespace Race
} // namespace Pulsar
