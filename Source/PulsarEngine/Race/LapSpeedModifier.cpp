#include <kamek.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/3D/Model/ModelDirector.hpp>
#include <MarioKartWii/Kart/KartValues.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <Race/200ccParams.hpp>
#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace Race {



//Max Lap Fix [Toadette Hack Fan]
kmWrite32(0x805328C0, 0x280000FF);
kmWrite32(0x805336c8, 0x280000FF);
kmWrite32(0x80534bcc, 0x280000FF);
kmWrite32(0x80534360, 0x280000FF);


RaceinfoPlayer* LoadCustomLapCount(RaceinfoPlayer* player, u8 id) {
//Mostly a port of MrBean's version with better hooks and arguments documentation
    const u8 lapKMP = KMP::Manager::sInstance->stgiSection->holdersArray[0]->raw->lapCount;
    u8 lapSetting = 3;
    u8 lapCount = lapKMP;

    switch ( System::sInstance->GetContext(LAPS_LAPS) ) {
        case LAPSETTING_LAPS_1: lapSetting = 1 ;break;
        case LAPSETTING_LAPS_2: lapSetting = 2 ;break;
        case LAPSETTING_LAPS_3: lapSetting = 3 ;break;
        case LAPSETTING_LAPS_4: lapSetting = 4 ;break;
        case LAPSETTING_LAPS_5: lapSetting = 5 ;break;
        case LAPSETTING_LAPS_6: lapSetting = 6 ;break;
        case LAPSETTING_LAPS_7: lapSetting = 7 ;break;
        case LAPSETTING_LAPS_8: lapSetting = 8 ;break;
        case LAPSETTING_LAPS_9: lapSetting = 9 ;break;
        case LAPSETTING_LAPS_15: lapSetting = 15 ;break;
        case LAPSETTING_LAPS_50: lapSetting = 50 ;break;
        case LAPSETTING_LAPS_255: lapSetting = 255 ;break;
        default: lapSetting = 3;
    }

    switch ( System::sInstance->GetFullRadioContext(LAP_MATHS) ) {
        case(LAPSETTING_CALC_EXCLUDE): // Exclusive
            if (lapKMP == 3) {
                lapCount = lapSetting;
            };
            break;
        case(LAPSETTING_CALC_FORCE): // Forced
            lapCount = lapSetting;
            break;
        default: // Calculated
            lapCount = ((lapKMP * lapSetting + 1) / 3); // added 1 for rounding
            if (lapCount < 1) lapCount = 1;
            if ((lapKMP * lapSetting + 1)/3 > 255) lapCount = 255;
        }

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

    // Speed Setting
    switch ( System::sInstance->GetContext(PHYS_SPEED) ) {
    case(PHYSSETTING_SPEED_110):
        factor = 1.1f;
        break;
    case(PHYSSETTING_SPEED_125):
        factor = 1.25f;
        break;
    case(PHYSSETTING_SPEED_150):
        factor = 1.5f;
        break;
    case(PHYSSETTING_SPEED_175):
        factor = 1.75f;
        break;
    case(PHYSSETTING_SPEED_200):
        factor = 2.0f;
        break;
    case(PHYSSETTING_SPEED_300):
        factor = 3.0f;
        break;
    case(PHYSSETTING_SPEED_500):
        factor = 5.0f;
        break;
    case(PHYSSETTING_SPEED_999):
        factor = 100.0f;
        break;
    case(PHYSSETTING_SPEED_070):
        factor = 0.7f;
        break;
    case(PHYSSETTING_SPEED_080):
        factor = 0.8f;
        break;
    case(PHYSSETTING_SPEED_090):
        factor = 0.9f;
        break;
    default:
        factor = 1.0f;
    }
    factor *= speedModConv.speedMod;

    // Lap-Based Speed Setting
    //const u8 lapCount = racedata->racesScenario.settings.lapCount; // <- This will be useful!
    //player->raceCompletion // <- This is our race completion
/*
    switch ( System::sInstance->GetContext(LAPS_SPEED) ) {
    case(LAPSETTING_SPEED_101):
        factor *= 1.01f;
        break;
    case(LAPSETTING_SPEED_105):
        factor *= 1.05f;
        break;
    case(LAPSETTING_SPEED_110):
        factor *= 1.1f;
        break;
    case(LAPSETTING_SPEED_125):
        factor *= 1.25f;
        break;
    case(LAPSETTING_SPEED_150):
        factor *= 1.5f;
        break;
    case(LAPSETTING_SPEED_200):
        factor *= 2.0f;
        break;
    case(LAPSETTING_SPEED_050):
        factor *= 0.5f;
        break;
    case(LAPSETTING_SPEED_075):
        factor *= 0.75f;
        break;
    case(LAPSETTING_SPEED_090):
        factor *= 0.9f;
        break;
    case(LAPSETTING_SPEED_095):
        factor *= 0.95f;
        break;
    case(LAPSETTING_SPEED_099):
        factor *= 0.99f;
        break;
    default: break;
*/

    Item::greenShellSpeed = 105.0f * factor;
    Item::redShellInitialSpeed = 75.0f * factor;
    Item::redShellSpeed = 130.0f * factor;
    Item::blueShellSpeed = 260.0f * factor;
    Item::blueShellMinimumDiveDistance = 640000.0f * factor;
    Item::blueShellHomingSpeed = 130.0f * factor;

    // credits to BlueLeopard02 for 200cc speed fix in the Pulsar discord
    if ( System::sInstance->GetBoolRadioContext(PHYS_SPEEDLIMIT) == PHYSSETTING_SPEEDLIMIT_DISABLED )
        Kart::hardSpeedCap = 340282346638528859811704200000000000000.0f;
        else Kart::hardSpeedCap = 120.0f + (factor - 1.0f) * 90.0f;
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