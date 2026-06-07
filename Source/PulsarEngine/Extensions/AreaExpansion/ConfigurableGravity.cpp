#include <MarioKartWii/Kart/KartPhysics.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Kart/KartSub.hpp>
#include <MarioKartWii/KMP/KMPManager.hpp>
#include <Extensions/AreaExpansion/KMPAREAExpander.hpp>

#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>

// Configurable Gravity [ImZeraora, BlueLeopard]
namespace MKWG {
namespace Race {

using namespace Pulsar;

void ConfigurableGravity(Kart::Physics& physics, u8 playerId) { // updates gravity for all gravity changing mechanics
    KMP::Manager* kmp = KMP::Manager::sInstance;
    KMPAREAExpander& KAE = KMPAREAExpand[playerId];

    if (KAE.configGrav) {
        KAE.upDown = (kmp->areaSection->GetHolder(KAE.ConfigurableGravity)->raw->setting1 == 0) ? -1.0f : 1.0f;
        KAE.AREAGravity = kmp->areaSection->GetHolder(KAE.ConfigurableGravity)->raw->setting2 / 100.0f;
        physics.gravity = KAE.upDown * KAE.AREAGravity;
    }
    if (KAE.glider) {
        KAE.upDown = (kmp->areaSection->GetHolder(KAE.Gliding)->raw->setting1 == 0) ? -1.0f : 1.0f;
        KAE.AREAGravity = kmp->areaSection->GetHolder(KAE.Gliding)->raw->setting2 / 100.0f;
    }
    
    if (KAE.underWater) {
        KAE.underWaterGravity = kmp->areaSection->GetHolder(KAE.Submarine)->raw->setting2 / 100.0f;
        physics.gravity = KAE.underWaterGravity - KAE.zInput * 0.10f;
    }

    // Okay so I get that the BlueLeopard doesn't want people editing the code
    // for area expansion but like I don't know if that extends to making it
    // compatible with other area codes since I'm not exactly changing either
    // code functions for what they're actually used for
    // Not to mention that BlueLeopard's code already ignores area 0xB so like
    // it's not like I'm changing any areas of BlueLeopard's here
    // Furthermore, gotta make this stuff compatible with the CDP gravity anyway
    // which also doesn't touch areas but does affect the same physics :P
    // crazy that I'm writing all this just because I'm concerned over two tracks...
    // goddamn it Hey Doctor Pepper and The Cylinder v2.0
    // Wait, fuck, I made both of those. Blast it all.
    // also wait this is a private pack why do I care and why would anyone else care

    switch ( System::sInstance->GetContext(PHYS_GRAVITY) ) {
        case(PHYSSETTING_GRAVITY_110): physics.gravity *= 1.1f   ;break;
        case(PHYSSETTING_GRAVITY_125): physics.gravity *= 1.25f  ;break;
        case(PHYSSETTING_GRAVITY_150): physics.gravity *= 1.5f   ;break;
        case(PHYSSETTING_GRAVITY_200): physics.gravity *= 2.0f   ;break;
        case(PHYSSETTING_GRAVITY_500): physics.gravity *= 5.0f   ;break;
        case(PHYSSETTING_GRAVITY_025): physics.gravity *= 0.25f  ;break;
        case(PHYSSETTING_GRAVITY_050): physics.gravity *= 0.5f   ;break;
        case(PHYSSETTING_GRAVITY_075): physics.gravity *= 0.75f  ;break;
        case(PHYSSETTING_GRAVITY_090): physics.gravity *= 0.9f   ;break;
        default: break;} // No need to multiply something by 1

    if (KAE.gravityConradi) {
        AREA* area = kmp->areaSection->GetHolder(KAE.GravityConradi)->raw;
        float* settingAsFloat = reinterpret_cast<float*>(&area->setting1);
        physics.gravity *= *settingAsFloat;
    }
}

} // Race
} // MKWG