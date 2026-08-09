#include <kamek.hpp>
#include <PulsarSystem.hpp>


// Credits to Insane Kart Wii


namespace Pulsar {

//Runtime WBZ Decoding (worst implementation ever)
extern "C" void DecoderEnd(void*);
asmFunc WBZDecoderPAL() {
    ASM(
    nofralloc;
loc_0x0:
  stwu      r1, -0x80(r1);
  mflr      r0;
  stw       r0, 0x84(r1);
  stmw      r3, 0x8(r1);
  bl        loc_0xC8;
  b         loc_0x190;

loc_0x18:
  lwz       r3, -0x5CA0(r13);
  lwz       r3, 0x24(r3);
  blr;       

loc_0x24:
  lwz       r12, 0x34(r3);
  mtctr     r12;
  bctr;      

loc_0x30:
  lwz       r3, 0x20(r3);
  blr;       

loc_0x38:
  mflr      r3;
  mtlr      r12;
  blr;       

loc_0x44:
  mflr      r12;
  bl        loc_0x38;
  opword    0x2f72656c;
  opword    0x2f72756e;
  andis.    r9, r3, 0x6D65;
  rlwnm     r23,r27,r12,9,29;
  rlwnm.    r4,r27,r12,21,17;
  xoris     r4, r27, 0x696E;
  oris      r31, r26, 0x502E;
  andi.     r5, r19, 0x6C00;

loc_0x6C:
  stwu      r1, -0x10(r1);
  mflr      r0;
  stw       r0, 0x14(r1);
  stw       r31, 0xC(r1);
  mr        r31, r3;
  andi.     r9, r3, 0x1F;
  beq-      loc_0x94;
  rlwinm    r31,r3,27,5,31;
  addi      r31, r31, 0x1;
  rlwinm    r31,r31,5,0,26;

loc_0x94:
  bl        loc_0x18;
  mr        r5, r3;
  li        r4, 0x20;
  mr        r3, r31;
  lis       r9, 0x8022;
  ori       r9, r9, 0x9814;
  mtctr     r9;
  bctrl;     
  lwz       r0, 0x14(r1);
  mtlr      r0;
  lwz       r31, 0xC(r1);
  addi      r1, r1, 0x10;
  blr;       

loc_0xC8:
  stwu      r1, -0x50(r1);
  mflr      r0;
  stw       r0, 0x54(r1);
  bl        loc_0x44;
  lis       r9, 0x8015;
  ori       r9, r9, 0xDF4C;
  mtctr     r9;
  bctrl;     
  addi      r4, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE254;
  mtctr     r9;
  bctrl;     
  cmpwi     r3, 0;
  bne-      loc_0x114;

loc_0x104:
  lwz       r0, 0x54(r1);
  mtlr      r0;
  addi      r1, r1, 0x50;
  blr;       

loc_0x114:
  stw       r31, 0x4C(r1);
  lwz       r3, 0x3C(r1);
  bl        loc_0x6C;
  mr        r31, r3;
  li        r7, 0x2;
  li        r6, 0;
  lwz       r5, 0x3C(r1);
  mr        r4, r3;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE834;
  mtctr     r9;
  bctrl;     
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE568;
  mtctr     r9;
  bctrl;     
  mr        r3, r31;
  bl        loc_0x30;
  bl        loc_0x6C;
  mr        r4, r3;
  mr        r3, r31;
  lis       r9, 0x801A;
  ori       r9, r9, 0x72DC;
  mtctr     r9;
  bctrl;     
  mr        r3, r31;
  bl        loc_0x24;
  lwz       r31, 0x4C(r1);
  b         loc_0x104;

loc_0x190:
  lmw       r3, 0x8(r1);
  lwz       r0, 0x84(r1);
  mtlr      r0;
  addi      r1, r1, 0x80;
  lwz       r4, 0x20(r26);
  b DecoderEnd;
    )
}

asmFunc WBZDecoderUSA() {
  ASM(
  nofralloc;
loc_0x0:
  stwu      r1, -0x80(r1);
  mflr      r0;
  stw       r0, 0x84(r1);
  stmw      r3, 0x8(r1);
  bl        loc_0xC8;
  b         loc_0x190;

loc_0x18:
  lwz       r3, -0x5CA8(r13);
  lwz       r3, 0x24(r3);
  blr;

loc_0x24:
  lwz       r12, 0x34(r3);
  mtctr     r12;
  bctr;

loc_0x30:
  lwz       r3, 0x20(r3);
  blr;

loc_0x38:
  mflr      r3;
  mtlr      r12;
  blr;       

loc_0x44:
  mflr      r12;
  bl        loc_0x38;
  opword    0x2f72656c;
  opword    0x2f72756e;
  andis.    r9, r3, 0x6D65;
  rlwnm     r23,r27,r12,9,29;
  rlwnm.    r4,r27,r12,21,17;
  xoris     r4, r27, 0x696E;
  oris      r31, r26, 0x452E;
  andi.     r5, r19, 0x6C00;

loc_0x6C:
  stwu      r1, -0x10(r1);
  mflr      r0;
  stw       r0, 0x14(r1);
  stw       r31, 0xC(r1);
  mr        r31, r3;
  andi.     r9, r3, 0x1F;
  beq-      loc_0x94;
  rlwinm    r31,r3,27,5,31;
  addi      r31, r31, 0x1;
  rlwinm    r31,r31,5,0,26;

loc_0x94:
  bl        loc_0x18;
  mr        r5, r3;
  li        r4, 0x20;
  mr        r3, r31;
  lis       r9, 0x8022;
  ori       r9, r9, 0x9490;
  mtctr     r9;
  bctrl;     
  lwz       r0, 0x14(r1);
  mtlr      r0;
  lwz       r31, 0xC(r1);
  addi      r1, r1, 0x10;
  blr;       

loc_0xC8:
  stwu      r1, -0x50(r1);
  mflr      r0;
  stw       r0, 0x54(r1);
  bl        loc_0x44;
  lis       r9, 0x8015;
  ori       r9, r9, 0xDEAC;
  mtctr     r9;
  bctrl;     
  addi      r4, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE1B4;
  mtctr     r9;
  bctrl;
  cmpwi     r3, 0;
  bne-      loc_0x114;

loc_0x104:
  lwz       r0, 0x54(r1);
  mtlr      r0;
  addi      r1, r1, 0x50;
  blr;       

loc_0x114:
  stw       r31, 0x4C(r1);
  lwz       r3, 0x3C(r1);
  bl        loc_0x6C;
  mr        r31, r3;
  li        r7, 0x2;
  li        r6, 0;
  lwz       r5, 0x3C(r1);
  mr        r4, r3;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE794;
  mtctr     r9;
  bctrl;     
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE4C8;
  mtctr     r9;
  bctrl;     
  mr        r3, r31;
  bl        loc_0x30;
  bl        loc_0x6C;
  mr        r4, r3;
  mr        r3, r31;
  lis       r9, 0x801A;
  ori       r9, r9, 0x723C;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x24;
  lwz       r31, 0x4C(r1);
  b         loc_0x104;

loc_0x190:
  lmw       r3, 0x8(r1);
  lwz       r0, 0x84(r1);
  mtlr      r0;
  addi      r1, r1, 0x80;
  lwz       r4, 0x20(r26);
  b DecoderEnd;
  )
}

asmFunc WBZDecoderJAP() {
  ASM(
  nofralloc;
loc_0x0:
  stwu      r1, -0x80(r1);
  mflr      r0;
  stw       r0, 0x84(r1);
  stmw      r3, 0x8(r1);
  bl        loc_0xC8;
  b         loc_0x190;

loc_0x18:
  lwz       r3, -0x5CA0(r13);
  lwz       r3, 0x24(r3);
  blr;

loc_0x24:
  lwz       r12, 0x34(r3);
  mtctr     r12;
  bctr;

loc_0x30:
  lwz       r3, 0x20(r3);
  blr;

loc_0x38:
  mflr      r3;
  mtlr      r12;
  blr;

loc_0x44:
  mflr      r12;
  bl        loc_0x38;
  opword    0x2f72656c;
  opword    0x2f72756e;
  andis.    r9, r3, 0x6D65;
  rlwnm     r23,r27,r12,9,29;
  rlwnm.    r4,r27,r12,21,17;
  xoris     r4, r27, 0x696E;
  oris      r31, r26, 0x4A2E;
  andi.     r5, r19, 0x6C00;

loc_0x6C:
  stwu      r1, -0x10(r1);
  mflr      r0;
  stw       r0, 0x14(r1);
  stw       r31, 0xC(r1);
  mr        r31, r3;
  andi.     r9, r3, 0x1F;
  beq-      loc_0x94;
  rlwinm    r31,r3,27,5,31;
  addi      r31, r31, 0x1;
  rlwinm    r31,r31,5,0,26;

loc_0x94:
  bl        loc_0x18;
  mr        r5, r3;
  li        r4, 0x20;
  mr        r3, r31;
  lis       r9, 0x8022;
  ori       r9, r9, 0x9734;
  mtctr     r9;
  bctrl;
  lwz       r0, 0x14(r1);
  mtlr      r0;
  lwz       r31, 0xC(r1);
  addi      r1, r1, 0x10;
  blr;

loc_0xC8:
  stwu      r1, -0x50(r1);
  mflr      r0;
  stw       r0, 0x54(r1);
  bl        loc_0x44;
  lis       r9, 0x8015;
  ori       r9, r9, 0xDE6C;
  mtctr     r9;
  bctrl;
  addi      r4, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE174;
  mtctr     r9;
  bctrl;
  cmpwi     r3, 0;
  bne-      loc_0x114;

loc_0x104:
  lwz       r0, 0x54(r1);
  mtlr      r0;
  addi      r1, r1, 0x50;
  blr;

loc_0x114:
  stw       r31, 0x4C(r1);
  lwz       r3, 0x3C(r1);
  bl        loc_0x6C;
  mr        r31, r3;
  li        r7, 0x2;
  li        r6, 0;
  lwz       r5, 0x3C(r1);
  mr        r4, r3;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE754;
  mtctr     r9;
  bctrl;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE488;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x30;
  bl        loc_0x6C;
  mr        r4, r3;
  mr        r3, r31;
  lis       r9, 0x801A;
  ori       r9, r9, 0x71FC;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x24;
  lwz       r31, 0x4C(r1);
  b         loc_0x104;

loc_0x190:
  lmw       r3, 0x8(r1);
  lwz       r0, 0x84(r1);
  mtlr      r0;
  addi      r1, r1, 0x80;
  lwz       r4, 0x20(r26);
  b DecoderEnd;
  )
}

asmFunc WBZDecoderKOR() {
  ASM(
  nofralloc;
loc_0x0:
  stwu      r1, -0x80(r1);
  mflr      r0;
  stw       r0, 0x84(r1);
  stmw      r3, 0x8(r1);
  bl        loc_0xC8;
  b         loc_0x190;

loc_0x18:
  lwz       r3, -0x5C80(r13);
  lwz       r3, 0x24(r3);
  blr;

loc_0x24:
  lwz       r12, 0x34(r3);
  mtctr     r12;
  bctr;

loc_0x30:
  lwz       r3, 0x20(r3);
  blr;

loc_0x38:
  mflr      r3;
  mtlr      r12;
  blr;

loc_0x44:
  mflr      r12;
  bl        loc_0x38;
  opword    0x2f72656c;
  opword    0x2f72756e;
  andis.    r9, r3, 0x6D65;
  rlwnm     r23,r27,r12,9,29;
  rlwnm.    r4,r27,r12,21,17;
  xoris     r4, r27, 0x696E;
  oris      r31, r26, 0x4B2E;
  andi.     r5, r19, 0x6C00;

loc_0x6C:
  stwu      r1, -0x10(r1);
  mflr      r0;
  stw       r0, 0x14(r1);
  stw       r31, 0xC(r1);
  mr        r31, r3;
  andi.     r9, r3, 0x1F;
  beq-      loc_0x94;
  rlwinm    r31,r3,27,5,31;
  addi      r31, r31, 0x1;
  rlwinm    r31,r31,5,0,26;

loc_0x94:
  bl        loc_0x18;
  mr        r5, r3;
  li        r4, 0x20;
  mr        r3, r31;
  lis       r9, 0x8022;
  ori       r9, r9, 0x9B88;
  mtctr     r9;
  bctrl;
  lwz       r0, 0x14(r1);
  mtlr      r0;
  lwz       r31, 0xC(r1);
  addi      r1, r1, 0x10;
  blr;

loc_0xC8:
  stwu      r1, -0x50(r1);
  mflr      r0;
  stw       r0, 0x54(r1);
  bl        loc_0x44;
  lis       r9, 0x8015;
  ori       r9, r9, 0xDFC4;
  mtctr     r9;
  bctrl;
  addi      r4, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE2CC;
  mtctr     r9;
  bctrl;
  cmpwi     r3, 0;
  bne-      loc_0x114;

loc_0x104:
  lwz       r0, 0x54(r1);
  mtlr      r0;
  addi      r1, r1, 0x50;
  blr;

loc_0x114:
  stw       r31, 0x4C(r1);
  lwz       r3, 0x3C(r1);
  bl        loc_0x6C;
  mr        r31, r3;
  li        r7, 0x2;
  li        r6, 0;
  lwz       r5, 0x3C(r1);
  mr        r4, r3;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE8AC;
  mtctr     r9;
  bctrl;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE5E0;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x30;
  bl        loc_0x6C;
  mr        r4, r3;
  mr        r3, r31;
  lis       r9, 0x801A;
  ori       r9, r9, 0x7638;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x24;
  lwz       r31, 0x4C(r1);
  b         loc_0x104;

loc_0x190:
  lmw       r3, 0x8(r1);
  lwz       r0, 0x84(r1);
  mtlr      r0;
  addi      r1, r1, 0x80;
  lwz       r4, 0x20(r26);
  b DecoderEnd;
  )
}

asmFunc WBZDecoderDEMO() {
  ASM(
  nofralloc;
loc_0x0:
  stwu      r1, -0x80(r1);
  mflr      r0;
  stw       r0, 0x84(r1);
  stmw      r3, 0x8(r1);
  bl        loc_0xC8;
  b         loc_0x190;

loc_0x18:
  lwz       r3, -0x5C98(r13);
  lwz       r3, 0x24(r3);
  blr;

loc_0x24:
  lwz       r12, 0x34(r3);
  mtctr     r12;
  bctr;

loc_0x30:
  lwz       r3, 0x20(r3);
  blr;

loc_0x38:
  mflr      r3;
  mtlr      r12;
  blr;

loc_0x44:
  mflr      r12;
  bl        loc_0x38;
  opword    0x2f72656c;
  opword    0x2f72756e;
  andis.    r9, r3, 0x6D65;
  rlwnm     r23,r27,r12,9,29;
  rlwnm.    r4,r27,r12,21,17;
  xoris     r4, r27, 0x696E;
  oris      r31, r26, 0x442E;
  andi.     r5, r19, 0x6C00;

loc_0x6C:
  stwu      r1, -0x10(r1);
  mflr      r0;
  stw       r0, 0x14(r1);
  stw       r31, 0xC(r1);
  mr        r31, r3;
  andi.     r9, r3, 0x1F;
  beq-      loc_0x94;
  rlwinm    r31,r3,27,5,31;
  addi      r31, r31, 0x1;
  rlwinm    r31,r31,5,0,26;

loc_0x94:
  bl        loc_0x18;
  mr        r5, r3;
  li        r4, 0x20;
  mr        r3, r31;
  lis       r9, 0x8022;
  ori       r9, r9, 0x9398;
  mtctr     r9;
  bctrl;
  lwz       r0, 0x14(r1);
  mtlr      r0;
  lwz       r31, 0xC(r1);
  addi      r1, r1, 0x10;
  blr;

loc_0xC8:
  stwu      r1, -0x50(r1);
  mflr      r0;
  stw       r0, 0x54(r1);
  bl        loc_0x44;
  lis       r9, 0x8015;
  ori       r9, r9, 0xDD1C;
  mtctr     r9;
  bctrl;
  addi      r4, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE024;
  mtctr     r9;
  bctrl;
  cmpwi     r3, 0;
  bne-      loc_0x114;

loc_0x104:
  lwz       r0, 0x54(r1);
  mtlr      r0;
  addi      r1, r1, 0x50;
  blr;

loc_0x114:
  stw       r31, 0x4C(r1);
  lwz       r3, 0x3C(r1);
  bl        loc_0x6C;
  mr        r31, r3;
  li        r7, 0x2;
  li        r6, 0;
  lwz       r5, 0x3C(r1);
  mr        r4, r3;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE604;
  mtctr     r9;
  bctrl;
  addi      r3, r1, 0x8;
  lis       r9, 0x8015;
  ori       r9, r9, 0xE338;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x30;
  bl        loc_0x6C;
  mr        r4, r3;
  mr        r3, r31;
  lis       r9, 0x801A;
  ori       r9, r9, 0x70AC;
  mtctr     r9;
  bctrl;
  mr        r3, r31;
  bl        loc_0x24;
  lwz       r31, 0x4C(r1);
  b         loc_0x104;

loc_0x190:
  lmw       r3, 0x8(r1);
  lwz       r0, 0x84(r1);
  mtlr      r0;
  addi      r1, r1, 0x80;
  lwz       r4, 0x20(r26);
  b DecoderEnd;
  )
}

void LoadCorrectCode() {
  switch ( *(char*)0x80000003 ) {
    case 'P': return WBZDecoderPAL();
    case 'E': return WBZDecoderUSA();
    case 'J': return WBZDecoderJAP();
    case 'K': return WBZDecoderKOR();
    case 'D': return WBZDecoderDEMO();
  }
}
kmBranch(0x8000A2A4, LoadCorrectCode);

}  // namespace Pulsar