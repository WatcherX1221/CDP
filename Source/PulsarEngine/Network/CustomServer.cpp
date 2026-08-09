#include <kamek.hpp>

#define _NEWWFC_

#include <kamek.hpp>

extern "C"
{
    extern s32 Patch_LoginPrintHeader;
    char *hash;
    char *code;
    char *gs;
    char *w0;
    char *pul2;
    
    static u32 rmce[] = {0x00000000, 0x00000000, 0x801d8f58, 0x801d91bc, 0x801d9258, 0x80286aa4, 0x802ed938};
    static u32 rmcj[] = {0x00000000, 0x00000000, 0x801D8F18, 0x801D917C, 0x801D9218, 0x8028A784, 0x802F1638};
    static u32 rmcp[] = {0x00000000, 0x00000000, 0x801D8FF8, 0x801D925C, 0x801D92F8, 0x8028ADE4, 0x802F1CB8};
    static u32 rmck[] = {0x00000000, 0x00000000, 0x801D9354, 0x801D95B8, 0x801D9654, 0x80278DD4, 0x802DFCB8};
    char *gamever = "RMCXDa2\0\0\0\0\0\x60\0\0\0";
    // u32 *destination = nullptr; // Destination address (example address)
}

static void writemem32(u32 offset, u32 value)
{
    memcpy((void *)offset, &value, sizeof(u32));
}

static int patch()
{

    static u32 data[] = {0x4800007C, 0x35306434, 0x38623033, 0x36326537,
                         0x33383763, 0x64353739, 0x37326162, 0x31643534,
                         0x35333239}; // payload hash

    u32 offsets[7] = {0};

    u32 codedata[] = {
        0x48000088,
        0x3FE0800F,
        // different
        0x00000000, // 0x3BFFDE0C, //2*4

        0x8061000C,
        0x38800F2A,
        0x38A1001C,
        0x48002D59,
        0x3861001C,
        0x389FFFDC,
        // diferent
        0x00000000, // 0x4BF24721,//9*4

        0x2C030000,
        0x4182000C,
        0x3800AE5A,
        0x48000A34,
        0x8061000C,
        0x38630002,
        0x389F0000,

        0x00000000, //0x80AD973C JPE, //issue, different in kor 17*4

        0x38A559E0,
        0x7C6903A6,
        0x4E800421,
        0x48000A68,
    };

    u32 *addr1 = (u32 *)codedata[2];
    u32 *addr2 = (u32 *)codedata[9];
    u32 *addr3 = (u32 *)codedata[17];
    // Get path
    // const char *path;
    switch (*(char *)0x80000003)
    {
    case 'E':
        gamever = "RMCEDf0\0\0\0\0\0";
        memcpy(offsets, rmce, sizeof(rmce));
        gs = (char *)0x80276a25;
        w0 = (char *)0x802760ec;
        code = (char *)0x800edff8;
        hash = (char *)0x800EDDE4;
        writemem32(0x800EE308, 0x4bfffcf4); // branches C60EE308 800EDFFC
        writemem32(0x800D1478, 0x480001f4); // branches C60D1478 800D166C
        codedata[2] = 0x3BFFDE0C;
        codedata[9] = 0x4BF24721;
        codedata[17] = 0x80AD973C;
        break;
    case 'J':
        gamever = "RMCJDf0\0\0\0\0\0";
        memcpy(offsets, rmcj, sizeof(rmcj));
        gs = (char *)0x8027A705;
        w0 = (char *)0x80279DCC;
        code = (char *)0x800EDFB8;
        hash = (char *)0x800EDDA4;
        writemem32(0x800ee2c8, 0x4bfffcf4); // C60EE2C8 800EDFBC 4B FF FC F4
        writemem32(0x800D1438, 0x480001f4); // C60D1438 800D162C 48 00 01 F4
        codedata[2] = 0x3BFFDDCC;           // fix
        codedata[9] = 0x4BF251E5;
        codedata[17] = 0x80AD973C;
        break;
    case 'P':
        gamever = "RMCPDf0\0\0\0\0\0";
        memcpy(offsets, rmcp, sizeof(rmcp));
        gs = (char *)0x8027A42C;
        w0 = (char *)0x8027A42C;
        code = (char *)0x800EE098;
        hash = (char *)0x800EDE84;
        writemem32(0x800EE3a8, 0x4bfffcf4); // C60EE3A8 800EE09C  4BFFFCF4
        writemem32(0x800D1518, 0x480001f4); // C60D1518 800D170C 480001F4
        codedata[2] = 0x3BFFDEAC;
        codedata[9] = 0x4BF251E1;
        codedata[17] = 0x80AD973C;
        break;
    case 'K':
        gamever = "RMCKDf0\0\0\0\0\0";
        memcpy(offsets, rmck, sizeof(rmck));
        gs = (char *)0x80268C15;
        w0 = (char *)0x802682DC;
        code = (char *)0x800EE110;
        hash = (char *)0x800EDEFC;
        writemem32(0x800EE420, 0x4bfffcf4); // C60EE420 800EE114 4B FF FC F4
        writemem32(0x800D1578, 0x480001f4); // C60D1578 800D176C 48 00 01 F4
        codedata[2] = 0x3BFFDF24;
        codedata[9] = 0x4BF251D1;
        codedata[17] = 0x80AD975C;
        break;
    }
    pul2 = (char *)0x800017CC;
    memcpy(code, codedata, sizeof(codedata));
    memcpy(hash, data, sizeof(data));
    memcpy(hash + sizeof(data), offsets, sizeof(offsets));
    memcpy(hash + sizeof(data) + sizeof(offsets), gamever, 12);
    strcpy(gs, "gs.newwfc.xyz");
    strcpy(w0, "://nas.newwfc.xyz/w0");
    strcpy(pul2, "PUL2");

    return 0;
};

kmOnLoad(patch);