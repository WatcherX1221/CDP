#include <PulsarSystem.hpp>
#include <Settings/SettingsParam.hpp>
#include <MarioKartWii/UI/Page/Menu/KartSelect.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>

// Credits to VP for Kart Restrictions

namespace Pulsar {
namespace UI {


// Changes the display type of the kart select depending on the kart restriction.
u32 RestrictKartSelection(){
    SectionMgr::sInstance->sectionParams->kartsDisplayType = 2;
    u8 kartRest = System::sInstance->GetFullRadioContext(HOST_KARTRESTRICT);
    if (kartRest == HOSTSETTING_RESTRICTKART_KARTONLY) SectionMgr::sInstance->sectionParams->kartsDisplayType = 0;
    else if (kartRest == HOSTSETTING_RESTRICTKART_BIKEONLY) SectionMgr::sInstance->sectionParams->kartsDisplayType = 1;
    SectionMgr::sInstance->sectionParams->kartsDisplayType = 2; // temporary line to disable setting behaviour because it isn't working
    return SectionMgr::sInstance->sectionParams->kartsDisplayType;
}
kmCall(0x808455a4, RestrictKartSelection);
kmWrite32(0x808455a8, 0x907f06ec);


// Removes karts from the accessible pool if restricted when selecting a kart in multiplayer.
bool IsKartAccessible(KartId kart, u32 r4){
    bool ret = IsKartUnlocked(kart, r4);
    u8 kartRest = System::sInstance->GetFullRadioContext(HOST_KARTRESTRICT);

    if ((kart < STANDARD_BIKE_S && kartRest == HOSTSETTING_RESTRICTKART_BIKEONLY) ||
        (kart >= STANDARD_BIKE_S && kartRest == HOSTSETTING_RESTRICTKART_KARTONLY)){
        ret = false;
    }

    return ret;
}
kmCall(0x8084a45c, IsKartAccessible);


} // namespace UI
} // namespace Pulsar