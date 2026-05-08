#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/Kumo.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <PulsarSystem.hpp>


namespace Pulsar {
namespace Race {
//Mega TC
void MegaTC(Kart::Movement& movement, int frames, int unk0, int unk1) {
    switch (System::sInstance->IsContext(WDD_TC_EFFECT)) {
        case(0x1): //Double Effect
            switch (System::sInstance->IsContext(WDD_TC_BIN1)+System::sInstance->IsContext(WDD_TC_BIN2)*2) {
                case(0x1): //Mega
                    movement.ActivateMega();
                    break;
                case(0x2): //Star
                    movement.ActivateStar();
                    break;
                case(0x3): //Mushroom
                    movement.ApplyInk(0);
                    break;
                default: //Default
                    movement.ApplyLightningEffect(frames, unk0, unk1);
            }
            switch (System::sInstance->IsContext(WDD_EXTRATC_BIN1)+System::sInstance->IsContext(WDD_EXTRATC_BIN2)*2) {
                case(0x1): //Mega
                    movement.ActivateMega();
                    break;
                case(0x2): //Star
                    movement.ActivateStar();
                    break;
                case(0x3): //Mushroom
                    movement.ApplyInk(0);
                    break;
                default: //Default
                    movement.ApplyLightningEffect(frames, unk0, unk1);
            }
        default: //Single Effect
            switch (System::sInstance->IsContext(WDD_TC_BIN1)+System::sInstance->IsContext(WDD_TC_BIN2)*2) {
                case(0x1): //Mega
                    movement.ActivateMega();
                    break;
                case(0x2): //Star
                    movement.ActivateStar();
                    break;
                case(0x3): //Mushroom
                    movement.ApplyInk(0);
                    break;
                default: //Default
                    movement.ApplyLightningEffect(frames, unk0, unk1);
            }
    }
}
kmCall(0x80580630, MegaTC);

void LoadCorrectTCBRRES(Item::ObjKumo& objKumo, const char* mdlName, const char* shadowSrc, u8 whichShadowListToUse,
    Item::Obj::AnmParam* anmParam) {
    switch (System::sInstance->IsContext(WDD_TC_BIN1)+System::sInstance->IsContext(WDD_TC_BIN2)*2) {
        case(0x1): //Mega
            objKumo.LoadGraphics("megaTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
        case(0x2): //Star
            objKumo.LoadGraphics("starTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
        case(0x3): //Blooper
            objKumo.LoadGraphics("blooperTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
        default: //Shock
            objKumo.LoadGraphicsImplicitBRRES(mdlName, shadowSrc, 1, anmParam, static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr);
    }
}
kmCall(0x807af568, LoadCorrectTCBRRES);


}//namespace Race
}//namespace Pulsar