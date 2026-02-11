#include <kamek.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/3D/Model/ModelDirector.hpp>
#include <MarioKartWii/Kart/KartValues.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <Race/200ccParams.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Race {
//Mostly a port of MrBean's version with better hooks and arguments documentation
RaceinfoPlayer* LoadCustomLapCount(RaceinfoPlayer* player, u8 id) {
    u8 lapCount = KMP::Manager::sInstance->stgiSection->holdersArray[0]->raw->lapCount;

    //Blockface3 Lap Mod

    int lapSetting = System::sInstance->IsContext(LOLPACK_LAPCOUNT_BIN8)*8
                   + System::sInstance->IsContext(LOLPACK_LAPCOUNT_BIN4)*4
                   + System::sInstance->IsContext(LOLPACK_LAPCOUNT_BIN2)*2
                   + System::sInstance->IsContext(LOLPACK_LAPCOUNT_BIN1)+3;
    if (lapSetting > 9) {
        lapSetting -= 9;
    };
    bool lapMax9 = 0;
    if ((lapSetting == 9) || (lapCount == 9)) {
        lapMax9 = 1;
    };
    switch (System::sInstance->IsContext(LOLPACK_LAPS_BIN1)+System::sInstance->IsContext(LOLPACK_LAPS_BIN2)*2) {
    case(0x1): // Exclusive
        if (lapCount == 3) {
            lapCount = lapSetting;
        };
        break;
    case(0x2): // Forced
        lapCount = lapSetting;
        break;
    case(0x3): // EXPONENT DEBUG
        lapCount = lapSetting*lapSetting*lapSetting;
        break;
    default: // Calculated
        lapCount = ((lapCount * lapSetting + 1) / 3); // added 1 for rounding
        if (lapCount < 1) lapCount = 1;
        else if ((lapCount > 9) && (lapMax9)) lapCount = 9;
        else if ((lapCount > 8) && (!lapMax9)) lapCount = 8;
    }

    //End of custom code

    Racedata::sInstance->racesScenario.settings.lapCount = lapCount;
    return new(player) RaceinfoPlayer(id, lapCount);
}
kmCall(0x805328d4, LoadCustomLapCount);

//kmWrite32(0x80723d64, 0x7FA4EB78);
void DisplayCorrectLap(AnmTexPatHolder* texPat) { //This Anm is held by a ModelDirector in a Lakitu::Player
    register u32 maxLap;
    asm(mr maxLap, r29;);
    texPat->UpdateRateAndSetFrame((float)(maxLap - 2));
    return;
}
kmCall(0x80723d70, DisplayCorrectLap);


//kmWrite32(0x808b5cd8, 0x3F800000); //change 100cc speed ratio to 1.0    
Kart::Stats* ApplySpeedModifier(KartId kartId, CharacterId characterId) {
    union SpeedModConv {
        float speedMod;
        u32 kmpValue;
    };

    Kart::Stats* stats = Kart::ComputeStats(kartId, characterId);
    SpeedModConv speedModConv;
    speedModConv.kmpValue = (KMP::Manager::sInstance->stgiSection->holdersArray[0]->raw->speedMod << 16);
    if(speedModConv.speedMod == 0.0f) speedModConv.speedMod = 1.0f;
    float factor = 1.0f;

    //Blockface3 Speed Mod

    switch ( System::sInstance->IsContext(LOLPACK_SPEEDMOD_BIN8)*8
           + System::sInstance->IsContext(LOLPACK_SPEEDMOD_BIN4)*4
           + System::sInstance->IsContext(LOLPACK_SPEEDMOD_BIN2)*2
           + System::sInstance->IsContext(LOLPACK_SPEEDMOD_BIN1)) {
    case(0x1):
        factor = 1.1f;
        break;
    case(0x2):
        factor = 1.25f;
        break;
    case(0x3):
        factor = 1.5f;
        break;
    case(0x4):
        factor = 1.75f;
        break;
    case(0x5):
        factor = 2.0f;
        break;
    case(0x6):
        factor = 3.0f;
        break;
    case(0x7):
        factor = 5.0f;
        break;
    case(0x8):
        factor = 100.0f;
        break;
    case(0x9):
        factor = 0.7f;
        break;
    case(0xA):
        factor = 0.8f;
        break;
    case(0xB):
        factor = 0.9f;
        break;
    default:
        factor = 1.0f;
    }
    factor *= speedModConv.speedMod;

    //End of custom code

    Item::greenShellSpeed = 105.0f * factor;
    Item::redShellInitialSpeed = 75.0f * factor;
    Item::redShellSpeed = 130.0f * factor;
    Item::blueShellSpeed = 260.0f * factor;
    Item::blueShellMinimumDiveDistance = 640000.0f * factor;
    Item::blueShellHomingSpeed = 130.0f * factor;

    Kart::hardSpeedCap = 120.0f + (factor - 1.0f) * 90.0f; // credits to BlueLeopard02 for 200cc speed fix in the Pulsar discord
    Kart::bulletSpeed = 145.0f * factor;
    Kart::starSpeed = 105.0f * factor;
    Kart::megaTCSpeed = 95.0f * factor;

    stats->baseSpeed *= factor;
    stats->standard_acceleration_as[0] *= factor;
    stats->standard_acceleration_as[1] *= factor;
    stats->standard_acceleration_as[2] *= factor;
    stats->standard_acceleration_as[3] *= factor;

    Kart::minDriftSpeedRatio = 0.55f * (factor > 1.0f ? (1.0f / factor) : 1.0f);
    Kart::unknown_70 = 70.0f * factor;
    Kart::regularBoostAccel = 3.0f * factor;

    return stats;
}
kmCall(0x8058f670, ApplySpeedModifier);

kmWrite32(0x805336B8, 0x60000000);
kmWrite32(0x80534350, 0x60000000);
kmWrite32(0x80534BBC, 0x60000000);
kmWrite32(0x80723D10, 0x281D0009);
kmWrite32(0x80723D40, 0x3BA00009);

kmWrite24(0x808AAA0C, 'PUL'); //time_number -> time_numPUL
}//namespace Race
}//namespace Pulsar