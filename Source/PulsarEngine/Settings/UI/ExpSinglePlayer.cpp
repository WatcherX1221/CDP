#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Menu/SinglePlayer.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <PulsarSystem.hpp>
#include <UI/UI.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
// Defines TT modes


namespace Pulsar {
static void SetCC();
namespace UI {



// hey remember that time I thought this was unused and I completely destroyed the entire main menu?
// that was funny af we gotta do that again sometime
// anyway so this is USED because of settings button
void CorrectButtonCount(Pages::SinglePlayer* page) {
//    const System* system = System::sInstance;
//    const bool hasFeather = system->GetInfo().HasFeather();
//    const bool has200cc = system->GetInfo().Has200cc();
//    page->externControlCount = 4 + hasFeather + has200cc + (hasFeather && has200cc) + 1;
    page->externControlCount = 5;
    new (page) Page;
}
kmCall(0x806266b8, CorrectButtonCount);
kmWrite32(0x806266d4, 0x60000000);

UIControl* CreateExternalControls(Pages::SinglePlayer* page, u32 id) {
    if(id == page->externControlCount - 1) {
        PushButton* button = new(PushButton);
        page->AddControl(page->controlCount++, *button, 0);
        const char* name =  "Settings1P";
        button->Load(UI::buttonFolder, name, name, page->activePlayerBitfield, 0, false);
        return button;
    }
    return page->Pages::SinglePlayer::CreateExternalControl(id);

}
kmWritePointer(0x808D9F84, CreateExternalControls);

//kmWrite32(0x8084f080, 0x7F89E378); //get idx in r9
//unused in lolpack (???)
static void LoadCorrectBRCTR(PushButton& button, const char* folder, const char* ctr, const char* variant, u32 localPlayerField) {
    register int idx;
    asm(mr idx, r28;);
    Pages::SinglePlayer* page = button.parentGroup->GetParentPage<Pages::SinglePlayer>();
    const System* system = System::sInstance;

    u32 varId = 0;
    u32 count = page->externControlCount;
    if(count > 5 && (idx == 1 || idx > 3)) {
        switch(count) {
            case(6):
                ctr = "PulTTTwo";
                if(idx != 1) {
                    if(system->GetInfo().Has200cc()) varId = 1;
                    else varId = 2;
                }
                break;
            case(8):
                ctr = "PulTTFour";
                if(idx != 1) varId = idx - 3;
                break;
        }
        char ttVariant[0x15];
//        snprintf(ttVariant, 0x15, "%s_%d", ctr, varId);
        variant = ttVariant;

    }

    button.Load(folder, ctr, variant, localPlayerField, 0, false);
    page->curMovieCount = 0; // Theoretically might be able to get this working now that we don't have the other tt buttons? not worth trying for now tho lol
}
kmCall(0x8084f084, LoadCorrectBRCTR);


//kmWrite32(0x8084f08c, 0x60000000);
//kmWrite32(0x8084f094, 0x38030001);
//kmWrite32(0x8084f098, 0x60000000);

//Hacky custom CalcDistance so that the navigating the single player menu is intuitive
//Still used for the settings button(?) so used in lolpack
static int FixCalcDistance(const ControlManipulator& subject, const ControlManipulator& other, Directions direction) {
    const s32 subId = static_cast<PushButton*>(subject.actionHandlers[0]->subject)->buttonId;
    const s32 destId = static_cast<PushButton*>(other.actionHandlers[0]->subject)->buttonId;
    switch(subId) {
        case(0):
            if(direction == DIRECTION_DOWN && destId == 1) return 1;
            break;
        case(2):
            if(direction == DIRECTION_UP && destId == 1) return 1;
            break;
        case(1):
        case(4):
        case(5):
        case(6):
            if(direction == DIRECTION_UP && destId == 0 || direction == DIRECTION_DOWN && destId == 2) return 1;
    }
    return subject.CalcDistanceBothWrapping(other, direction);
}

static void SetDistanceFunc(ControlsManipulatorManager& mgr) {
    mgr.distanceFunc = &FixCalcDistance;
}
kmCall(0x8084ef68, SetDistanceFunc);

//Sets bmg message at the bottom of the screen
//function is completely redone from scratch
void OnButtonSelect(Pages::SinglePlayer* page, PushButton& button, u32 hudSlotId) {
    const s32 id = button.buttonId;
    u32 bmgId;
    if(id == 4) bmgId = BMG_SETTINGSBUTTON_BOTTOM;
    else if(id == 1) {
        bmgId = BMG_TT_MODE_BOTTOM_SINGLE;
        if(!System::sInstance->GetBoolRadioContext(TTS_VALID)) bmgId+=4;
        else {
            const u8 speedContext = System::sInstance->GetContext(PHYS_SPEED);
            const bool itemBool   = System::sInstance->GetBoolRadioContext(ITEM_START_ENABLED);
            const u8 itemContext  = System::sInstance->GetContext(ITEM_START);
            switch (speedContext) {
                case PHYSSETTING_SPEED_100: break;
                case PHYSSETTING_SPEED_150: bmgId +=1; break;
                default:
                    bmgId = BMG_TT_MODE_BOTTOM_SINGLE + 4;
                    page->bottomText->SetMessage(bmgId);
                    return;
            }
            if (itemBool) {
                switch (itemContext) {
                    case ITEMSETTING_START_3MUS: break;
                    case ITEMSETTING_START_3FEA: bmgId +=2; break;
                    default:
                        bmgId = BMG_TT_MODE_BOTTOM_SINGLE + 4;
                        page->bottomText->SetMessage(bmgId);
                        return;
                }
            } // Else item = 3MUS, which is valid and shows correct BMG.
        }
        page->bottomText->SetMessage(bmgId);
    }
    else page->Pages::SinglePlayer::OnExternalButtonSelect(button, hudSlotId);
}
kmWritePointer(0x808D9F64, &OnButtonSelect);



//Sets the ttMode based on which button was clicked
//Except it actually doesn't anymore, instead opting to use lolpack settings due to trophy validation
//We still need to set ttmode though for ghost folders
void OnButtonClick(Pages::SinglePlayer* page, PushButton& button, u32 hudSlotId) {
    System* system = System::sInstance;
    const u32 id = button.buttonId;
//    if(page->externControlCount > 4 && id == page->externControlCount - 1) {
    if(id == 4) {
        ExpSection::GetSection()->GetPulPage<SettingsPanel>()->prevPageId = PAGE_SINGLE_PLAYER_MENU;
        page->nextPageId = static_cast<PageId>(SettingsPanel::id);
        page->EndStateAnimated(0, button.GetAnimationFrameSize());
        return;
    }
    //if(id == 5) { // Fix wiimote back button click (to be done, don't know how to do this??)

    if(id == 1 || id > 3) button.buttonId = 1;
    page->Pages::SinglePlayer::OnButtonClick(button, hudSlotId);
    button.buttonId = id;
    if(id == 1 || id > 3) {
//        TTMode mode = TTMODE_150;
//        switch(page->externControlCount) {
//            case(6):
//                if(id > 3) {
//                    if(system->GetInfo().Has200cc()) mode = TTMODE_200;
//                    else mode = TTMODE_150_FEATHER;
//                }
//                break;
//            case(8):
//                if(id > 3) mode = (TTMode)(id - 3);
//                break;
//        }

        // Convert settings into standard TT modes

        const u8 speedContext = System::sInstance->GetContext(PHYS_SPEED);
        const bool itemBool   = System::sInstance->GetBoolRadioContext(ITEM_START_ENABLED);
        const u8 itemContext  = System::sInstance->GetContext(ITEM_START);

        TTMode mode = TTMODE_UNRESTRICTED;
        if(System::sInstance->GetBoolRadioContext(TTS_VALID)) {
            if(itemContext == ITEMSETTING_START_3MUS || itemBool == false) {
                if(speedContext==PHYSSETTING_SPEED_100) mode = TTMODE_150;
                else mode = TTMODE_200;
            }
            else if(itemContext == ITEMSETTING_START_3FEA) {
                if(speedContext==PHYSSETTING_SPEED_100) mode = TTMODE_150_FEATHER;
                else mode = TTMODE_200_FEATHER;
            }
        }
        system->ttMode = mode;
        //SetCC();
    }
}
kmWritePointer(0x808BBED0, OnButtonClick);
}//namespace UI

//Sets the CC (based on the mode) when retrying after setting a time, as racedata's CC is overwritten usually //Actually useless now since lolpack 200cc overrides 150cc
static void SetCC() {
    const System* system = System::sInstance;
    EngineClass cc = CC_150;
    if(system->ttMode == TTMODE_200 || system->ttMode == TTMODE_200_FEATHER) cc = CC_100;
    Racedata::sInstance->menusScenario.settings.engineClass = cc;
}
//kmBranch(0x805e1ef4, SetCC);
//kmBranch(0x805e1d58, SetCC);

}//namespace Pulsar