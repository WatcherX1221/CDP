#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/Obj/Kumo.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/Kart/KartCollision.hpp>
#include <PulsarSystem.hpp>
#include <Settings/SettingsParam.hpp>


namespace Pulsar {
namespace Race {
//Mega TC
void MegaTC(Kart::Movement& movement, Kart::Collision& collision, int frames, int unk0, int unk1) {
    switch ( System::sInstance->IsContextWDD(ITEM_CLOUD_1)
           + System::sInstance->IsContextWDD(ITEM_CLOUD_2)*2
           + System::sInstance->IsContextWDD(ITEM_CLOUD_4)*4
           + System::sInstance->IsContextWDD(ITEM_CLOUD_8)*8
           ) {
        case ITEMSETTING_CLOUD_MEGA: // Mega
            movement.ActivateMega();
            break;
        case ITEMSETTING_CLOUD_STAR: // Star
            movement.ActivateStar();
            break;
        case ITEMSETTING_CLOUD_FEATHER: // Feather
            //movement.ActivateMushroom();
            //ActivateFeather();
            break;
        case ITEMSETTING_CLOUD_DEATH: // Death
            collision.ActivateOob(true, 0, true, 0);
            break;
        case ITEMSETTING_CLOUD_BLOOPER: // Blooper
            movement.ApplyInk(0);
            break;
        case ITEMSETTING_CLOUD_MEGASTAR: // Mega Star
            movement.ActivateMega();
            movement.ActivateStar();
            break;
        case ITEMSETTING_CLOUD_SHOCKBLOOPER: // Shock Blooper
            movement.ApplyLightningEffect(frames, unk0, unk1);
            movement.ApplyInk(0);
            break;
        case ITEMSETTING_CLOUD_SHOCKMEGA: // Dizzy
            movement.ApplyLightningEffect(frames, unk0, unk1);
            movement.ActivateMega();
            break;
        case ITEMSETTING_CLOUD_MEGASHOCK: // Cloud
            //summon enderdragon ~~~
            break;
        default: // Shock
            movement.ApplyLightningEffect(frames, unk0, unk1);
    }
}
kmCall(0x80580630, MegaTC);

void LoadCorrectTCBRRES(Item::ObjKumo& objKumo, const char* mdlName, const char* shadowSrc, u8 whichShadowListToUse,
    Item::Obj::AnmParam* anmParam) {
    switch ( System::sInstance->IsContextWDD(ITEM_CLOUD_1)
           + System::sInstance->IsContextWDD(ITEM_CLOUD_2)*2
           + System::sInstance->IsContextWDD(ITEM_CLOUD_4)*4
           + System::sInstance->IsContextWDD(ITEM_CLOUD_8)*8
           ) {
        case ITEMSETTING_CLOUD_MEGA: // Mega
            objKumo.LoadGraphics("megaTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
        case ITEMSETTING_CLOUD_STAR: // Star
            objKumo.LoadGraphics("starTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
//        case ITEMSETTING_CLOUD_FEATHER: // Feather
//            objKumo.LoadGraphics("featherTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
//        case ITEMSETTING_CLOUD_DEATH: // Death
//            objKumo.LoadGraphics("deathTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
        case ITEMSETTING_CLOUD_BLOOPER: // Blooper
            objKumo.LoadGraphics("blooperTC.brres", mdlName, shadowSrc, 1, anmParam,
            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
            break;
//        case ITEMSETTING_CLOUD_MEGASTAR: // Mega Star
//            objKumo.LoadGraphics("megastarTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
//        case ITEMSETTING_CLOUD_SHOCKBLOOPER: // Shock Blooper
//            objKumo.LoadGraphics("shockblooperTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
//        case ITEMSETTING_CLOUD_SHOCKMEGA: // Dizzy
//            objKumo.LoadGraphics("dizzyTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
//        case ITEMSETTING_CLOUD_MEGASHOCK: // Cloud
//            objKumo.LoadGraphics("cloudTC.brres", mdlName, shadowSrc, 1, anmParam,
//            static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr, 0);
//            break;
        default: //Shock
            objKumo.LoadGraphicsImplicitBRRES(mdlName, shadowSrc, 1, anmParam, static_cast<nw4r::g3d::ScnMdl::BufferOption>(0), nullptr);
    }
}
kmCall(0x807af568, LoadCorrectTCBRRES);


}//namespace Race
}//namespace Pulsar
