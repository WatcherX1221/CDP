#include <kamek.hpp>
#include <IKW.hpp>
#include <MarioKartWii/Race/Raceinfo/RaceInfo.hpp>
#include <Settings/SettingsBinary.hpp>
#include <Settings/Settings.hpp>
#include <Settings/MissionRunMgr.hpp>
#include <UI/UI.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <RuntimeWrite.hpp>

// Ported from IKW


namespace Pulsar {
namespace MissionMode {

// No Record Saving on Competitions / Tournament [B_squo]
kmWrite32(0x8085df04, 0x48000020);

extern "C" void MissionModeEnd1(void*);
extern "C" void sInstance__10SectionMgr(void*);
extern "C" void MissionModeStuff(void*);

asmFunc MissionMode1() {
  ASM(
  nofralloc;
  lis       r3, sInstance__10SectionMgr@ha;
  lwz       r3, sInstance__10SectionMgr@l(r3);
  lwz       r3, 0x0(r3);
  lwz       r3, 0x1AC(r3);
  li        r4, -0x1;
  stw       r4, 0x6B4(r3);
  lis       r4, 0x8000;
  ori       r4, r4, 0x9CC;
  stw       r4, 0x6B8(r3);
  lwz       r3, 0x668(r3);
  stw       r3, -0x8(r4);
  li        r3, 0;
  stw       r3, -0xC(r4);
  lis       r3, MissionModeStuff@h;
  ori       r3, r3, MissionModeStuff@l;
  mtctr     r3;
  mr        r3, r31;
  li        r0, 0x8E;
  bctr;
  b MissionModeEnd1;
  )
}
kmBranch(0x800009C8, MissionMode1);

kmRegionWrite32(0x8062D338, 0x4B9D3690, 'P');
kmRegionWrite32(0x8062D47C, 0x4B9D354C, 'P');
kmRegionWrite32(0x8062D5C0, 0x4B9D3408, 'P');
kmRegionWrite32(0x8062D80C, 0x4B9D31BC, 'P');
kmRegionWrite32(0x805FC484, 0x4BA04544, 'E');
kmRegionWrite32(0x805FC5C8, 0x4BA04400, 'E');
kmRegionWrite32(0x805FC70C, 0x4BA042BC, 'E');
kmRegionWrite32(0x805FC958, 0x4BA04070, 'E');
kmRegionWrite32(0x8062CA84, 0x4B9D3F44, 'J');
kmRegionWrite32(0x8062CBC8, 0x4B9D3E00, 'J');
kmRegionWrite32(0x8062CD0C, 0x4B9D3CBC, 'J');
kmRegionWrite32(0x8062CF58, 0x4B9D3A70, 'J');
kmRegionWrite32(0x8061B730, 0x4B9E5298, 'K');
kmRegionWrite32(0x8061B874, 0x4B9E5154, 'K');
kmRegionWrite32(0x8061B9B8, 0x4B9E5010, 'K');
kmRegionWrite32(0x8061BC04, 0x4B9E4DC4, 'K');

kmWrite32(0x800009CC, 0x4E800020);

extern "C" void MissionModeEnd3(void*);
asmFunc MissionMode3() {
  ASM(
  nofralloc;
  lis       r10, 0x8000;
  lwz       r9, 0x9D0(r10);
  cmpwi     r9, 0x1;
  bne-      loc_0x1C;
  li        r9, 0;
  stw       r9, 0x9D0(r10);
  li        r4, 0x4;

loc_0x1C:
  cmpwi     r4, 0x1;
  b MissionModeEnd3;
  )
}
kmBranch(0x8084F4AC, MissionMode3);

extern "C" void CreateAndInitPage__7SectionF6PageId(void*);
extern "C" void MissionModeEnd4(void*);
asmFunc MissionMode4() {
  ASM(
  nofralloc;
  lis       r3, CreateAndInitPage__7SectionF6PageId@ha;
  addi      r3, r3, CreateAndInitPage__7SectionF6PageId@l;
  mtctr     r3;
  mr        r3, r31;
  li        r4, 0x35;
  bctrl;     
  lis       r3, CreateAndInitPage__7SectionF6PageId@ha;
  addi      r3, r3, CreateAndInitPage__7SectionF6PageId@l;
  mtctr     r3;
  mr        r3, r31;
  li        r4, 0x2A;
  bctrl;     
  mr        r3, r31;
  b MissionModeEnd4;
  )
}
kmBranch(0x8062CA50, MissionMode4);

asmFunc MissionMode5() {
  ASM(
  nofralloc;
  lis       r3, sInstance__10SectionMgr@ha;
  lwz       r3, sInstance__10SectionMgr@l(r3);
  lwz       r3, 0x0(r3);
  lwz       r3, 0x0(r3);
  cmpwi     r3, 0x2C;
  beq+      loc_0x20;
  li        r3, 0x26;
  b         loc_0x24;

loc_0x20:
  li        r3, 0x25;

loc_0x24:
  blr;
  )
}
kmBranch(0x8085E3B4, MissionMode5);

extern "C" void MissionModeEnd6(void*);
asmFunc MissionMode6() {
  ASM(
  nofralloc;
  lis       r30, 0x8000;
  cmplw     r4, r30;
  bgt+      loc_0x10;
  lis       r4, 0x8170;

loc_0x10:
  mr        r30, r3;
  b MissionModeEnd6;
  )
}
kmBranch(0x80529E1C, MissionMode6);

extern "C" void MissionModeEnd7(void*);
asmFunc MissionMode7() {
  ASM(
  nofralloc;
  lis       r4, 0x8000;
  lwz       r4, 0x9C0(r4);
  cmpwi     r4, 0;
  beq+      loc_0x1C;
  mr        r3, r4;
  li        r0, 0;
  b         loc_0x3C;

loc_0x1C:
  cmpwi     r3, 0;
  beq-      loc_0x2C;
  lwz       r3, 0x0(r3);
  b         loc_0x3C;

loc_0x2C:
  lis       r3, 0x8170;
  li        r4, 0x1;
  stb       r4, 0x16(r3);
  li        r0, 0;

loc_0x3C:
  b MissionModeEnd7;
  )
}
kmBranch(0x8084302C, MissionMode7);

extern "C" void MissionModeEnd8(void*);
extern "C" void AddPageLayer__7SectionF6PageId(void*);
asmFunc MissionMode8() {
  ASM(
  nofralloc;
  lis       r3, AddPageLayer__7SectionF6PageId@h;
  ori       r3, r3, AddPageLayer__7SectionF6PageId@l;
  mtctr     r3;
  mr        r3, r31;
  bctrl;     
  lis       r3, AddPageLayer__7SectionF6PageId@h;
  ori       r3, r3, AddPageLayer__7SectionF6PageId@l;
  mtctr     r3;
  mr        r3, r31;
  li        r4, 0x7A;
  bctrl;
  b MissionModeEnd8;
  )
}
kmBranch(0x80630814, MissionMode8);

extern "C" void sInstance__8Racedata(void*);
asmFunc CPUsInMissionMode() {
  ASM(
  lhz       r7, 0x68(r3);
  cmpwi     r7, 0xB;
  ble+      loc_0x10;
  li        r7, 0xB;

loc_0x10:
  lis       r29, sInstance__8Racedata@ha;
  lwz       r29, sInstance__8Racedata@l(r29);
  addi      r29, r29, 0xC20;
  li        r8, 0x5;
  stw       r8, 0xF8(r29);
  stw       r8, 0x1E8(r29);
  stw       r8, 0x2D8(r29);
  stw       r8, 0x3C8(r29);
  stw       r8, 0x4B8(r29);
  stw       r8, 0x5A8(r29);
  stw       r8, 0x698(r29);
  stw       r8, 0x788(r29);
  stw       r8, 0x878(r29);
  stw       r8, 0x968(r29);
  stw       r8, 0xA58(r29);
  addi      r8, r3, 0x68;

loc_0x50:
  cmpwi     r7, 0;
  beq-      loc_0x80;
  addi      r29, r29, 0xF0;
  addi      r8, r8, 0x2;
  lbz       r9, 0x0(r8);
  stw       r9, 0x4(r29);
  lbz       r9, 0x1(r8);
  stw       r9, 0x0(r29);
  li        r9, 0x1;
  stw       r9, 0x8(r29);
  subi      r7, r7, 0x1;
  b         loc_0x50;

loc_0x80:
  lhz       r7, 0x68(r3);
  blr;
  )
}
kmCall(0x808431C8, CPUsInMissionMode);

//Fix Mission Mode Section [Conradi]
extern "C" void MissionModeFixEnd(void*);
asmFunc MissionModeFix() {
  ASM(
  nofralloc;
  cmpwi     r3, 0x4D;
  bne-      loc_0x10;
  li        r3, 0x1;
  blr;

loc_0x10:
  subi      r0, r3, 0x1E;
  b MissionModeFixEnd;
  )
}
kmBranch(0x80860450, MissionModeFix);

//Mission Mode File Renames [Toadette Hack Fan]
kmWrite16(0x808A8818, 0x7331);
kmWrite16(0x808A88E4, 0x7331);
kmWrite16(0x808A8B14, 0x7331);
kmWrite16(0x808A8BA5, 0x7331);
kmWrite16(0x808AD56A, 0x7331);
kmWrite32(0x8088FDAC, 0x676C312E);

//Print Mission Mode ID to Memory [Toadette Hack Fan]
extern "C" void MissionIDsEnd(void*);
asmFunc MissionModeIDs() {
  ASM(
  nofralloc;
  lis       r12, 0x8000;
  ori       r12, r12, 0x3E2;
  stb       r5, 0x0(r12);
  stwu      r1, -0x120(r1);
  b MissionIDsEnd;
  )
}
kmBranch(0x80540918, MissionModeIDs);


//Print MissionID to Memory [Toadette Hack Fan]
asmFunc MissionIDsMenu() {
  ASM(
    nofralloc;
    mulli r6, r7, 10;
    lis   r10, 0x8000;
    ori   r10, r10, 0x3E3;
    stb   r4, 0x0(r10);
    addi  r10, r10, 1;
    stb   r7, 0x0(r10);
    blr;
  )
}
kmCall(0x80842f04, MissionIDsMenu);

void PrintTimerToMemory() {
  if(U16_MISSION_MODE_FIX != 0x1) return;
  if(Raceinfo::sInstance->stage == RACESTAGE_FINISHED) return;
  u32 frames = Raceinfo::sInstance->timerMgr->raceFrameCounter;
  if(frames < 65000) U32_TIMER = frames * 1001/60;
}
static RaceFrameHook PrintTimerHook(PrintTimerToMemory);


u8 rank = 0;
void MissionModeRankings() {
  if(U16_MISSION_MODE_FIX != 0x1) return;
  if(Settings::MissionRunMgr::sInstance == nullptr) return;
  U8_MISSION_ID = U8_MISSION_WORLD * 8 + U8_MISSION_LEVEL;
  
  rank = 0;
  static const u32 rankThresholds[64][6] = {
    // World 1
    {80000, 60000, 50000, 40000, 35000, 30000},     // Mission 0
    {80000, 65000, 60000, 55000, 50000, 45000},     // Mission 1
    {60000, 53000, 47000, 42000, 38000, 35000},     // Mission 2
    {75000, 64000, 55000, 50000, 45000, 40000},     // Mission 3
    {105000, 95000, 90000, 85000, 80000, 75000},    // Mission 4
    {60000, 50000, 41000, 32000, 25000, 18000},     // Mission 5
    {100000, 90000, 85000, 75000, 70000, 65000},    // Mission 6
    {90000, 75000, 60000, 45000, 35000, 27000},     // Mission 7
    // World 2
    {90000, 83000, 76000, 70000, 65000, 60000},     // Mission 8
    {40000, 32000, 26000, 22000, 18000, 15000},     // Mission 9
    {90000, 83000, 76000, 70000, 65000, 60000},     // Mission 10
    {50000, 43000, 36000, 30000, 25000, 20000},     // Mission 11
    {75000, 64000, 55000, 50000, 45000, 40000},     // Mission 12
    {75000, 65000, 56000, 47000, 38000, 30000},     // Mission 13
    {90000, 79000, 70000, 65000, 60000, 50000},     // Mission 14
    {120000, 110000, 100000, 95000, 90000, 85000},  // Mission 15
    // World 3
    {70000, 65000, 60000, 56000, 53000, 50000},     // Mission 16
    {100000, 85000, 75000, 65000, 55000, 45000},    // Mission 17
    {90000, 75000, 65000, 55000, 45000, 35000},     // Mission 18
    {135000, 130000, 125000, 120000, 117000, 115000}, // Mission 19
    {75000, 72000, 70000, 68000, 66500, 65000},     // Mission 20
    {90000, 80000, 70000, 60000, 50000, 46500},     // Mission 21
    {100000, 92000, 86000, 82000, 78000, 75000},    // Mission 22
    {140000, 135000, 130000, 126000, 123000, 120000}, // Mission 23
    // World 4
    {120000, 115000, 110000, 106000, 103000, 100000}, // Mission 24
    {45000, 37000, 31000, 27000, 23000, 20000},     // Mission 25
    {80000, 69000, 60000, 55000, 50000, 45000},     // Mission 26
    {93000, 86000, 79000, 73000, 68000, 63000},     // Mission 27
    {90000, 85000, 80000, 76000, 73000, 70000},     // Mission 28
    {100000, 95000, 90000, 86000, 83000, 80000},    // Mission 29
    {90000, 80000, 71000, 62000, 53000, 45000},     // Mission 30
    {90000, 73000, 60000, 50000, 40000, 30000},     // Mission 31
    // World 5
    {70000, 59000, 50000, 45000, 40000, 35000},     // Mission 32
    {90000, 80000, 71000, 62000, 53000, 45000},     // Mission 33
    {120000, 110000, 101000, 92000, 83000, 75000},  // Mission 34
    {100000, 89000, 80000, 75000, 70000, 65000},    // Mission 35
    {110000, 105000, 95000, 85000, 80000, 75000},   // Mission 36
    {55000, 45000, 42000, 39000, 37000, 35000},     // Mission 37
    {120000, 110000, 107000, 104000, 102000, 100000}, // Mission 38
    {150000, 140000, 135000, 131500, 129000, 127000}, // Mission 39
    // World 6
    {90000, 80000, 75000, 71000, 68000, 65000},     // Mission 40
    {90000, 78000, 72000, 68000, 64000, 60000},     // Mission 41
    {100000, 85000, 75000, 65000, 55000, 45000},    // Mission 42
    {0, 30000, 40000, 50000, 70000, 90000},         // Mission 43 (reverse)
    {80000, 68000, 62000, 57000, 52000, 46500},     // Mission 44
    {120000, 100000, 92000, 88000, 84000, 80000},   // Mission 45
    {120000, 108000, 102000, 98000, 94000, 90000},  // Mission 46
    {130000, 125000, 120000, 115000, 112000, 110000}, // Mission 47
    // World 7
    {100000, 90000, 85000, 81000, 78000, 75000},    // Mission 48
    {60000, 48000, 42000, 38000, 34000, 31000},     // Mission 49
    {60000, 48000, 43000, 40000, 36000, 33500},     // Mission 50
    {55000, 45000, 40000, 36000, 33000, 30000},     // Mission 51
    {90000, 85000, 79000, 74000, 71000, 68000},     // Mission 52
    {60000, 49000, 43000, 40000, 36000, 32500},     // Mission 53
    {90000, 85000, 82000, 79000, 77000, 74500},     // Mission 54
    {150000, 140000, 130000, 125000, 120000, 115500}, // Mission 55
    // World 8
    {45000, 40000, 38000, 36500, 35000, 34000},     // Mission 56
    {75000, 64000, 59000, 55000, 51000, 48500},     // Mission 57
    {90000, 80000, 76000, 73000, 70000, 68000},     // Mission 58
    {90000, 85000, 82000, 79000, 76000, 74000},     // Mission 59
    {75000, 65000, 61000, 58000, 55000, 53000},     // Mission 60
    {120000, 110000, 105000, 99000, 94000, 90000},  // Mission 61
    {120000, 115000, 111000, 107000, 103000, 100000},    // Mission 62
    {270000, 265000, 258000, 252000, 245000, 240000}, // Mission 63
  };

  if(U8_MISSION_ID >= 64) return;

  // Mission 6-4 is reversed
  if(U8_MISSION_ID == 43) {
    rank = 1;
    if(U32_TIMER > rankThresholds[43][1]) rank = 2;
    if(U32_TIMER > rankThresholds[43][2]) rank = 3;
    if(U32_TIMER > rankThresholds[43][3]) rank = 4;
    if(U32_TIMER > rankThresholds[43][4]) rank = 5;
    if(U32_TIMER > rankThresholds[43][5]) rank = 6;
  } else {
    // Normal logic for the other 63 missions
    for(int i = 0; i < 6; i++) {
      if(U32_TIMER < rankThresholds[U8_MISSION_ID][i]) {
        rank = i + 1;
      }
    }
  }

  static bool canSave = true;
  if(canSave && U8_CURRENT_PAGE == 0x25) {
    u8 oldRank = Settings::MissionRunMgr::sInstance->GetMissionRank(U8_MISSION_ID);
    if(rank > oldRank) {
      Settings::MissionRunMgr::sInstance->SetMissionRank(U8_MISSION_ID, rank);
      System::sInstance->taskThread->Request(&Settings::MissionRunMgr::SaveTask, nullptr, 0);
    }
    canSave = false;
  }
  else(canSave = true);
  static bool showMessage = true;
  if(Raceinfo::sInstance != nullptr) {
    if(Raceinfo::sInstance->stage == RACESTAGE_FINISHED && showMessage && U8_CURRENT_PAGE == PAGE_MISSION_TOURNAMENT_HUD) {
      Pages::MessageBoxTransparent* messageBox = SectionMgr::sInstance->curSection->Get<Pages::MessageBoxTransparent>();
      messageBox->Reset();
      messageBox->SetMessageWindowText(0x4FFF0 + rank, nullptr);
      Pulsar::UI::ExpSection* panel = static_cast<Pulsar::UI::ExpSection*>(SectionMgr::sInstance->curSection);
      Pulsar::UI::ExpSection::AddPageLayer(*panel, PAGE_MESSAGE_BOX_TRANSPARENT);
      showMessage = false;
      return;
    }
    if(U8_CURRENT_PAGE == PAGE_MISSION_ENDMENU) showMessage = true;
  }
  if(U8_CURRENT_PAGE == PAGE_MISSION_INFORMATION_PROMPT && (WiiInput == 0x0010 || GCInput == 0x1080 || ClassicInput == 0x400)) {
      const u8 readRank = Settings::MissionRunMgr::sInstance->GetMissionRank(U8_MISSION_ID);
      Text::Info info;
      info.bmgToPass[0] = U8_MISSION_ID + 0x4FF00;
      info.bmgToPass[1] = readRank + 0x4FFE0;
      Pages::MessageBoxTransparent* messageBox = SectionMgr::sInstance->curSection->Get<Pages::MessageBoxTransparent>();
      messageBox->Reset();
      messageBox->SetMessageWindowText(0x50000, &info);
      Pulsar::UI::ExpSection* panel = static_cast<Pulsar::UI::ExpSection*>(SectionMgr::sInstance->curSection);
      Pulsar::UI::ExpSection::AddPageLayer(*panel, PAGE_MESSAGE_BOX_TRANSPARENT);
      return;
    }
}
static PageLoadHook2 MissionModeRankingsHook(MissionModeRankings);

//Customize Unlock Screen [B-squo]
kmRuntimeUse(0x805eb920);
kmWrite32(0x80854158, 0x60000000);
kmWrite32(0x808541a4, 0x60000000);
kmWrite32(0x808542f8, 0x60000000);
kmWrite32(0x80854754, 0x38002E30);
kmWrite32(0x80854758, 0x900303e4);
kmWrite32(0x8085475C, 0x4e800020);
kmWrite32(0x808541c4, 0x3860001C);
kmWrite32(0x80854a44, 0x3860001C);
kmWrite32(0x80854e94, 0x38602E31);

//Button Count from Mii Outfit C
kmRuntimeUse(0x807E2714);
bool shouldUnlock = false;
void MiiOutfitCUnlockCheck() {
  if(Settings::MissionRunMgr::sInstance == nullptr) return;
  
  shouldUnlock = true;
  for(int i = 0; i < 64; i++) {
    if(Settings::MissionRunMgr::sInstance->GetMissionRank(i) < 4) {
      shouldUnlock = false;
      break;
    }
  }
  kmRuntimeWrite32A(0x805eb920, 0x48269685);
  if(REGION == 'E') kmRuntimeWrite32A(0x805eb920, 0x48226EFD);
  if(REGION == 'J') kmRuntimeWrite32A(0x805eb920, 0x48269415);
  if(REGION == 'K') kmRuntimeWrite32A(0x805eb920, 0x482698A9);
  if(shouldUnlock && Settings::MissionRunMgr::sInstance->GetUnlockFlag() != 1) kmRuntimeWrite32A(0x805eb920, 0x38600092);
}
static PageLoadHook MiiOutfitCUnlockHook(MiiOutfitCUnlockCheck);

void WriteUnlockFlagToSettingsFile() {
  if(Settings::MissionRunMgr::sInstance == nullptr) return;
  if(U8_CURRENT_PAGE == PAGE_UNLOCK) {
    Settings::MissionRunMgr::sInstance->SetUnlockFlag(1);
    System::sInstance->taskThread->Request(&Settings::MissionRunMgr::SaveTask, nullptr, 0);
  }
  if(Settings::MissionRunMgr::sInstance->GetUnlockFlag() == 1) kmRuntimeWrite32A(0x807E2714, 0x3BC0001B);
}
static PageLoadHook2 UnlockFlagHook(WriteUnlockFlagToSettingsFile);

void FlushMissionModeValuesOnBoot() {
  U8_MISSION_WORLD = 0;
  U8_MISSION_LEVEL = 0;
  U16_MISSION_MODE_FIX = 0;
}
BootHook FlushDaRAM(FlushMissionModeValuesOnBoot, 10);

}//namespace MissionMode
}//namespace Pulsar