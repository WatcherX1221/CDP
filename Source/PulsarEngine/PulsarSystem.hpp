#ifndef _PULSAR_
#define _PULSAR_

#include <kamek.hpp>
#include <core/egg/mem/ExpHeap.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/UI/Text/Text.hpp>
#include <Extensions/LECODE/LECODEMgr.hpp>
#include <Debug/Debug.hpp>
#include <IO/IO.hpp>
#include <Info.hpp>
#include <Config.hpp>
#include <Network/Network.hpp>
#include <Network/MatchCommand.hpp>


namespace Pulsar {
namespace KO {
class Mgr;
}//namespace KO

class ConfigFile;


enum RadioContexts {
    PULSAR_CT = 0x0,
    HOST_HAW = 0x1, // 0x2
    RACE_MIIHEADS = 0x3,
    MODE_OTT = 0x4,
    MODE_KO = 0x5,
    PHYS_BRAKE = 0x6,
    HOST_DISREGARD = 0x7,
    HOST_KARTRESTRICT = 0x8, // 0x2
    HOST_CHARRESTRICT = 0xA, // 0x2
    PHYS_TURBO = 0xC, // 0x2
    TTS_VALID = 0xE,
    ITEM_START_ENABLED = 0xF,
    LAP_MATHS = 0x10, // 0x2
    HOST_RACETIME = 0x12,
    PHYS_SPEEDLIMIT = 0x13
};

enum ScrollContexts {
    LAPS_LAPS,
    PHYS_SPEED,
    PHYS_GRAVITY,
    PHYS_KARTBIN,
    ITEM_ROULETTEBIN,
    ITEM_START,
    ITEM_CLOUDEFFECT
};


class System {
protected:
    System();
private:
    //System functions
    void Init(const ConfigFile& conf);
    void InitInstances(const ConfigFile& conf, IOType type);
    void InitIO(IOType type) const;
    void InitCups(const ConfigFile& conf);
    void InitSettings(const u16* totalTrophyCount) const;
    void UpdateContext();
protected:
    //Virtual
    virtual void AfterInit() {};
public:
    static System* sInstance;

    virtual void SetUserInfo(Network::ResvInfo::UserInfo& userInfo) {};
    virtual bool CheckUserInfo(const Network::ResvInfo::UserInfo& userInfo) { return true; };
    //Deprecated because you can now freely expand ROOM packets and do what you need to with them
    //virtual u8 SetPackROOMMsg() { return 0; } //Only called for hosts
    //virtual void ParsePackROOMMsg(u8 msg) {}  //Only called for non-hosts
    const Info& GetInfo() const { return this->info; }

    bool GetBoolRadioContext(RadioContexts context) const { return (this->radioContexts & (1 << context)) != 0; }
    u8   GetFullRadioContext(RadioContexts context) const { return (this->radioContexts >> context) & 0b11; }
    u8 GetContext(ScrollContexts context) const {
        switch (context) {
            case LAPS_LAPS: return this->lapCount;
            case PHYS_SPEED: return this->speedMod;
            case PHYS_GRAVITY: return this->gravMod;
            case PHYS_KARTBIN: return this->kartBin;
            case ITEM_ROULETTEBIN: return this->rouletteBin;
            case ITEM_START: return this->itemStart;
            case ITEM_CLOUDEFFECT: return this->cloudEffect;
            // This default should never run
            default: return this->radioContexts;
        }
    }
    static s32 OnSceneEnter(Random& random);

    const char* GetModFolder() const { return modFolderName; }
    static void CreateSystem();

    //Network
    static asmFunc GetRaceCount();

    //Modes
    static asmFunc GetNonTTGhostPlayersCount();

    //BMG
    const BMGHolder& GetBMG() const { return customBmgs; }
    /*
    #define PatchRegion(addr)\
        static inline u64 GetWiimmfiRegionStatic##addr(u64 src) {\
            register const Info *info = &System::sInstance->GetInfo();\
            asmVolatile(lwz r7, Info.wiimmfiRegion(info););\
            return src;\
        };\
        kmBranch(addr, GetWiimmfiRegionStatic##addr);\
        kmPatchExitPoint(GetWiimmfiRegionStatic##addr, ##addr + 4);
    */
    //VARIABLES
    EGG::ExpHeap* const heap; //0x4
    EGG::TaskThread* const taskThread; //0x8
    //Constants

private:
    char modFolderName[IOS::ipcMaxFileName + 1]; //0xC
    u8 padding[2];
    Info info; //0x1c
    u32 radioContexts;
    u8 lapCount;
    u8 speedMod;
    u8 gravMod;
    u8 kartBin;
    u8 rouletteBin;
    u8 itemStart;
    u8 cloudEffect;

    //Add extra contexts here

public:
    //Network variables only set when reading a ROOM packet that starts the GP; they are only ever used in UpdateState; no need to clear them as ROOM will reupdat ethem
    Network::Mgr netMgr;

    TTMode ttMode;

    //LECODE data
    LECODE::Mgr lecodeMgr;

    //Modes
    KO::Mgr* koMgr;
    u32 ottVoteState;
    bool ottHideNames;
    u8 nonTTGhostPlayersCount; //because a ghost can be added in vs, racedata's playercount is not reliable

private:
    //Custom BMGS
    BMGHolder customBmgs;
    BMGHeader* rawBmg;

public:
    //string pool
    static const char pulsarString[];
    static const char CommonAssets[];
    static const char breff[];
    static const char breft[];
    static const char* ttModeFolders[];

    struct Inherit {
        //static_assert(is_base_of<System, Child>::value, "Pulsar::System is not a parent of your class");
        typedef System* (*CreateFunc)();
        Inherit(CreateFunc func) {
            //static_assert(inherit == nullptr, "Can only inherit once from Pulsar::System");
            create = func;
            inherit = this;
        }
        CreateFunc create;
    };
    static Inherit* inherit;
    friend class Info;
};
} //namespace Pulsar

#endif