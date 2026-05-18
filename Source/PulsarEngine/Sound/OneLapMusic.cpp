#include <kamek.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/Audio/RaceMgr.hpp>
#include <MarioKartWii/Audio/RSARPlayer.hpp>
#include <MarioKartWii/Audio/Actors/KartActor.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <Settings/Settings.hpp>

// Code by Lanny

/*
One Lap Track Final Lap Music:
When playing on a 1 lap track, the final lap music will start playing
when the first local player reaches 2/3 (66.67%) completion of the track.
This makes 1 lap tracks feel more engaging by having the music speed up
at the final stretch of the race.

Respects MISC_RADIO_SPEEDUP setting:
- When enabled: gradually increases pitch instead of transitioning to _f file
- When disabled: transitions to _f file normally
*/

namespace Pulsar {
namespace Sound {

using namespace nw4r;

// Track whether we've already triggered the final lap music for this race
static bool hasTriggeredOneLapFinalMusic = false;
// Track whether we've already played the jingle (for speedup mode)
static bool hasPlayedOneLapJingle = false;

// Check if we should trigger final lap music on a 1 lap track
static void CheckOneLapFinalMusic(Audio::RaceMgr* raceMgr) {
    const Racedata* racedata = Racedata::sInstance;
    if (racedata == nullptr) return;

    // If users wish not to hear final lap music on 1 lap track then disable
    if (Settings::Mgr::Get().GetUserSettingValue(Settings::SETTINGSTYPE_AUDIO, SETTINGAUDIO_RADIO_LAPFINAL) == AUDIOSETTING_LAPFINAL_DISABLED) return;
    
    const u8 lapCount = racedata->racesScenario.settings.lapCount;
    
    // Only apply to 1 lap tracks
    if (lapCount != 1) return;
    
    const Raceinfo* raceinfo = Raceinfo::sInstance;
    if (raceinfo == nullptr || raceinfo->players == nullptr) return;
    
    const u8 hudPlayerId = racedata->racesScenario.settings.hudPlayerIds[0];
    if (hudPlayerId >= 12) return;
    
    RaceinfoPlayer* player = raceinfo->players[hudPlayerId];
    if (player == nullptr) return;
    
    // Check if player has completed 2/3 of the track (66.67%)
    // raceCompletion starts at 1.0 at lap start and goes to 2.0 for lap 1 completion
    // So for a 1 lap track: 1.0 = start, 2.0 = finish
    // 2/3 completion = 1.0 + (2/3 * 1.0) = 1.6667
    const float twoThirdsCompletion = 1.0f + (2.0f / 3.0f); // ~1.6667
    
    if (player->raceCompletion < twoThirdsCompletion) return;
    
    // Get the speedup setting
    u8 isSpeedUp = Settings::Mgr::Get().GetUserSettingValue(Settings::SETTINGSTYPE_AUDIO, SETTINGAUDIO_RADIO_SPEEDUP);
    
    // Get the kart sound for pitch manipulation
    Audio::KartActor* kartActor = raceMgr->kartActors[0];
    if (kartActor == nullptr) return;
    
    snd::detail::BasicSound& sound = kartActor->soundArchivePlayer->soundPlayerArray[0].soundList.GetFront();
    
    if (isSpeedUp == AUDIOSETTING_SPEEDUP_ENABLED) {
        // Speedup mode: gradually increase pitch, capped to avoid runaway
        float& pitch = sound.ambientParam.pitch;
        const float maxPitch = 1.05f; // cap like a short burst then flatten
        if (pitch < maxPitch) {
            pitch += 0.0002f;
            if (pitch > maxPitch) pitch = maxPitch;
        }
        
        // Play the final lap jingle once
        if (!hasPlayedOneLapJingle) {
            // Use the global RSAR player (same pattern as BRSTMSpeedUp)
            Audio::RSARPlayer* rsarPlayer = Audio::RSARPlayer::sInstance;
            if (rsarPlayer != nullptr) {
                rsarPlayer->PlaySound(SOUND_ID_FINAL_LAP, 0);
            }
            hasPlayedOneLapJingle = true;
        }
    }
    else {
        // Normal mode: transition to fast music
        if (!hasTriggeredOneLapFinalMusic) {
            if (raceMgr->raceState == Audio::RACE_STATE_NORMAL || raceMgr->raceState == Audio::RACE_STATE_FINISHED) {
                raceMgr->SetRaceState(Audio::RACE_STATE_FAST);
            }
            hasTriggeredOneLapFinalMusic = true;
        }
    }
}

static void CalcWithOneLapMusic(Audio::RaceMgr* raceMgr) {
    // Reset flags when race state goes back to intro/countdown
    if (raceMgr->raceState < Audio::RACE_STATE_NORMAL) {
        hasTriggeredOneLapFinalMusic = false;
        hasPlayedOneLapJingle = false;
    }
    
    // Call the original Calc function
    raceMgr->Calc();
    
    // Only check when race is in progress
    if (raceMgr->raceState == Audio::RACE_STATE_NORMAL || raceMgr->raceState == Audio::RACE_STATE_FAST) {
        CheckOneLapFinalMusic(raceMgr);
    }
}
kmCall(0x807165e8, CalcWithOneLapMusic);

} // namespace Sound
} // namespace Pulsar
