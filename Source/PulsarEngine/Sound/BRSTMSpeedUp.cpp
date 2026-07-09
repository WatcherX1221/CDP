#include <kamek.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/Audio/RSARPlayer.hpp>
#include <MarioKartWii/Audio/RaceMgr.hpp>
#include <MarioKartWii/Audio/Actors/KartActor.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceGhostDiffTime.hpp>
#include <Settings/Settings.hpp>


/*Music speedup:
When the player reaches the final lap (if the track has >1 laps) and if the setting is set, the music will
speedup instead of transitioning to the _f file. The jingle will still play.
*/

namespace Pulsar {
namespace Sound {

using namespace nw4r;
static void MusicSpeedup(Audio::RaceRSARPlayer* rsarSoundPlayer, u32 jingle, u8 hudSlotId) {
    //static u8 hudSlotIdFinalLap;
    // get Pulsar speedup setting
    u8 isSpeedUp = Settings::Mgr::Get().GetUserSettingValue(Settings::SETTINGSTYPE_AUDIO, SETTINGAUDIO_RADIO_SPEEDUP);

/*
    // get BRSTM internal speedup setting at byte 0x3E
    register snd::detail::StrmFileLoader* fileLoader;
    const u8 internalSpeedUp = *reinterpret_cast<const u8*>(ut::AddU32ToPtr(fileLoader, 0x3E));
    //const u8 internalSpeedUp = *reinterpret_cast<const u8*>(ut::AddU32ToPtr(fileLoader.fileReader.header, 0x3E));
    //asm(subi strmSound, r29, 0x100);

    // This is a modified version of the code at https://mariokartwii.com/showthread.php?tid=1948
    // If 0x3E = 0x00, use pulsar setting - Ignore
    // If 0x3E = 0x01, never speedup the BRSTM - set setting to disabled
    if( internalSpeedUp == 0x01 ) isSpeedUp = AUDIOSETTING_SPEEDUP_DISABLED;
    // If 0x3E = 0x02, always speedup the BRSTM - set setting to enabled
    else if( internalSpeedUp == 0x02 ) isSpeedUp = AUDIOSETTING_SPEEDUP_ENABLED;
*/

    Audio::RaceMgr* raceAudioMgr = Audio::RaceMgr::sInstance;
    const u8 maxLap = raceAudioMgr->maxLap;
    const u8 curLap = raceAudioMgr->lap;
    const RacedataSettings& raceDataSettings = Racedata::sInstance->racesScenario.settings;
    //const u8 idFirstFinalLap = hudSlotIdFinalLap;
    if(maxLap == 1) return;
    if(maxLap == raceDataSettings.lapCount) {

        register Audio::KartActor* kartActor;
        asm(mr kartActor, r29;);
        snd::detail::BasicSound& sound =  kartActor->soundArchivePlayer->soundPlayerArray[0].soundList.GetFront();
        if(isSpeedUp == AUDIOSETTING_SPEEDUP_ENABLED || sound.soundId == SOUND_ID_GALAXY_COLOSSEUM) {
            const Raceinfo* raceInfo = Raceinfo::sInstance;
            const Timer& raceTimer = raceInfo->timerMgr->timers[0];
            const Timer& playerTimer = raceInfo->players[raceDataSettings.hudPlayerIds[hudSlotId]]->lapSplits[maxLap - 2];
            const Timer difference = CtrlRaceGhostDiffTime::SubtractTimers(raceTimer, playerTimer);
            if(difference.minutes < 1 && difference.seconds < 5) {
                sound.ambientParam.pitch += 0.0002f;
            }
            if(maxLap != curLap) rsarSoundPlayer->PlaySound(SOUND_ID_FINAL_LAP, hudSlotId);
        }
        else if((maxLap != curLap) && (raceAudioMgr->raceState == 0x4 || raceAudioMgr->raceState == 0x6)) {
            raceAudioMgr->SetRaceState(Audio::RACE_STATE_FAST);
        }
    }
    else if(maxLap != curLap) {
        rsarSoundPlayer->PlaySound(SOUND_ID_NORMAL_LAP, hudSlotId);
        //hudSlotIdFinalLap = raceAudioMgr->playerIdFirstLocalPlayer;
    }
    return;
}
kmCall(0x8070b2f8, MusicSpeedup);
kmWrite32(0x8070b2c0, 0x60000000);
kmWrite32(0x8070b2d4, 0x60000000);

}//namespace Sound
}//namespace Pulsar