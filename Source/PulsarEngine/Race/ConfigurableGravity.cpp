#include <kamek.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Effect/EffectMgr.hpp> 
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/KMP/KMPManager.hpp>
#include <Race/200ccParams.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Race {


static const u8 AREA_TYPE_GRAVITY = 0x0b; // AREA type for configurable gravity

float GetAreaGravityMultiplier(u8 playerId) {
    float gravityMultiplier = 1.0f;
    
    Kart::Manager* kartMgr = Kart::Manager::sInstance;
    KMP::Manager* kmpMgr = KMP::Manager::sInstance;
    
    if (kartMgr == nullptr || kmpMgr == nullptr || kmpMgr->areaSection == nullptr) {
        return gravityMultiplier;
    }
    
    if (playerId >= kartMgr->playerCount) {
        return gravityMultiplier;
    }
    
    Kart::Player* player = kartMgr->players[playerId];
    if (player == nullptr) {
        return gravityMultiplier;
    }
    
    const Vec3& position = player->GetPhysics().position;
    
    s16 foundIdx = kmpMgr->FindAREA(position, -1, AREA_TYPE_GRAVITY);
    
    if (foundIdx >= 0) {
        AREA* area = kmpMgr->areaSection->holdersArray[foundIdx]->raw;
        float* settingAsFloat = reinterpret_cast<float*>(&area->setting1);
        gravityMultiplier = *settingAsFloat;
    }
    
    return gravityMultiplier;
}

static void ApplyAreaGravityMultiplier(Kart::Physics& physics, float dt, float maxSpeed, int air) {
    float gravityMultiplier = 1.0f;
    
    KMP::Manager* kmpMgr = KMP::Manager::sInstance;
    if (kmpMgr != nullptr && kmpMgr->areaSection != nullptr) {
        const Vec3& position = physics.position;
        
        s16 foundIdx = kmpMgr->FindAREA(position, -1, AREA_TYPE_GRAVITY);
        
        if (foundIdx >= 0) {
            AREA* area = kmpMgr->areaSection->holdersArray[foundIdx]->raw;
            float* settingAsFloat = reinterpret_cast<float*>(&area->setting1);
            gravityMultiplier = *settingAsFloat;
        }
    }

    switch ( System::sInstance->IsContextLOL(PHYS_GRAVITY_8)*8
           + System::sInstance->IsContextLOL(PHYS_GRAVITY_4)*4
           + System::sInstance->IsContextLOL(PHYS_GRAVITY_2)*2
           + System::sInstance->IsContextLOL(PHYS_GRAVITY_1)) {
        case(0x1): gravityMultiplier *= 1.1f   ;break;
        case(0x2): gravityMultiplier *= 1.25f  ;break;
        case(0x3): gravityMultiplier *= 1.5f   ;break;
        case(0x4): gravityMultiplier *= 2.0f   ;break;
        case(0x5): gravityMultiplier *= 5.0f   ;break;
        case(0x6): gravityMultiplier *= 0.25f  ;break;
        case(0x7): gravityMultiplier *= 0.5f   ;break;
        case(0x8): gravityMultiplier *= 0.75f  ;break;
        case(0x9): gravityMultiplier *= 0.9f   ;break;
        default: break;} // No need to multiply something by 1

    physics.gravity *= gravityMultiplier;
    
    physics.Update(false, dt, maxSpeed);
}
kmCall(0x8059fb5c, ApplyAreaGravityMultiplier);

}
}