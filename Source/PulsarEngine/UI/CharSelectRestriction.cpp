#include <MarioKartWii/UI/Ctrl/Menu/CtrlMenuCharacterSelect.hpp>
#include <MarioKartWii/UI/Page/Menu/CharacterSelect.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <PulsarSystem.hpp>
#include <Settings/SettingsParam.hpp>

// Credits to VP for Character Restrictions

namespace Pulsar {
namespace UI {

    enum WeightClass{
        LIGHTWEIGHT,
        MEDIUMWEIGHT,
        HEAVYWEIGHT,
        MIIS
    };

    enum CharButtonId{
        BUTTON_BABY_MARIO,
        BUTTON_BABY_LUIGI,
        BUTTON_TOAD,
        BUTTON_TOADETTE,
        BUTTON_BABY_PEACH,
        BUTTON_BABY_DAISY,
        BUTTON_KOOPA_TROOPA,
        BUTTON_DRY_BONES,
        BUTTON_MARIO,
        BUTTON_LUIGI,
        BUTTON_YOSHI,
        BUTTON_BIRDO,
        BUTTON_PEACH,
        BUTTON_DAISY,
        BUTTON_DIDDY_KONG,
        BUTTON_BOWSER_JR,
        BUTTON_WARIO,
        BUTTON_WALUIGI,
        BUTTON_KING_BOO,
        BUTTON_ROSALINA,
        BUTTON_DONKEY_KONG,
        BUTTON_FUNKY_KONG,
        BUTTON_BOWSER,
        BUTTON_DRY_BOWSER,
        BUTTON_MII_A,
        BUTTON_MII_B
    };

    // Uses the global function to get the character ID of the local player's Mii to determine it's weight class.
    WeightClass GetMiiWeightClass(Mii &mii){
        CharacterId charId = GetMiiCharacterId(mii);
        if (charId < MII_M_A_MALE){
            return LIGHTWEIGHT;
        }
        else if (charId < MII_L_A_MALE){
            return MEDIUMWEIGHT;
        }
        return HEAVYWEIGHT;
    }

    // "Enables" all the buttons on the character select screen by setting the images to the character panes and making all the buttons accessible.
    void EnableButtons(CtrlMenuCharacterSelect &charSelect){
        for (u8 i = 0; i < 42; i++)
        {
            CtrlMenuCharacterSelect::ButtonDriver *buttonDriver = charSelect.GetButtonDriver(static_cast<CharacterId>(i));
            if (buttonDriver != 0){
                buttonDriver->SetPicturePane("chara", GetCharacterIconPaneName(static_cast<CharacterId>(i)));
                buttonDriver->SetPicturePane("chara_shadow", GetCharacterIconPaneName(static_cast<CharacterId>(i)));
                buttonDriver->SetPicturePane("chara_light_01", GetCharacterIconPaneName(static_cast<CharacterId>(i)));
                buttonDriver->SetPicturePane("chara_light_02", GetCharacterIconPaneName(static_cast<CharacterId>(i)));
                buttonDriver->SetPicturePane("chara_c_down", GetCharacterIconPaneName(static_cast<CharacterId>(i)));
                buttonDriver->manipulator.inaccessible = false;
            }
        }
    }

    // Disables a specified button by setting the pane to a question mark and making the button inaccessible.
    void DisableButton(CtrlMenuCharacterSelect::ButtonDriver *button){
        button->SetPicturePane("chara", "cha_26_hatena");
        button->SetPicturePane("chara_shadow", "cha_26_hatena");
        button->SetPicturePane("chara_light_01", "cha_26_hatena");
        button->SetPicturePane("chara_light_02", "cha_26_hatena");
        button->SetPicturePane("chara_c_down", "cha_26_hatena");

        button->manipulator.inaccessible = true;
    }

    WeightClass GetWeightClass(CharacterId currentChar) {
        switch (currentChar) {
            case MARIO: return MEDIUMWEIGHT;
            case BABY_PEACH: return LIGHTWEIGHT;
            case WALUIGI: return HEAVYWEIGHT;
            case BOWSER: return HEAVYWEIGHT;
            case BABY_DAISY: return LIGHTWEIGHT;
            case DRY_BONES: return LIGHTWEIGHT;
            case BABY_MARIO: return LIGHTWEIGHT;
            case LUIGI: return MEDIUMWEIGHT;
            case TOAD: return LIGHTWEIGHT;
            case DONKEY_KONG: return HEAVYWEIGHT;
            case YOSHI: return MEDIUMWEIGHT;
            case WARIO: return HEAVYWEIGHT;
            case BABY_LUIGI: return LIGHTWEIGHT;
            case TOADETTE: return LIGHTWEIGHT;
            case KOOPA_TROOPA: return LIGHTWEIGHT;
            case DAISY: return MEDIUMWEIGHT;
            case PEACH: return MEDIUMWEIGHT;
            case BIRDO: return MEDIUMWEIGHT;
            case DIDDY_KONG: return MEDIUMWEIGHT;
            case KING_BOO: return HEAVYWEIGHT;
            case BOWSER_JR: return MEDIUMWEIGHT;
            case DRY_BOWSER: return HEAVYWEIGHT;
            case FUNKY_KONG: return HEAVYWEIGHT;
            case ROSALINA: return HEAVYWEIGHT;
            default: return LIGHTWEIGHT;
        }
    }

    void RestrictCharacterSelection(PushButton *button, u32 hudSlotId){
        Pages::CharacterSelect *page = SectionMgr::sInstance->curSection->Get<Pages::CharacterSelect>();
        CtrlMenuCharacterSelect &charSelect = page->ctrlMenuCharSelect;
        SectionId curSection = SectionMgr::sInstance->curSection->sectionId;
        u8 charRestrict = System::sInstance->GetFullRadioContext(HOST_CHARRESTRICT);
        CtrlMenuCharacterSelect::ButtonDriver *driverButtons = charSelect.driverButtonsArray;
        CharacterId currentChar = page->models[hudSlotId].curCharacter;
        WeightClass weight = GetWeightClass(currentChar);
        WeightClass miiWeight = GetMiiWeightClass(page->localPlayerMiis[0]);
        CtrlMenuCharacterSelect::ButtonDriver *newButton = charSelect.GetButtonDriver(currentChar);

        EnableButtons(charSelect);

        // Disables the buttons that are not in the character weight class restriction.
        if (charRestrict != HOSTSETTING_RESTRICTCHAR_DISABLED){
            for (int i = BUTTON_BABY_MARIO; i < BUTTON_MII_A; i++)
            {
                driverButtons[i].manipulator.inaccessible = false;
                if (charRestrict == HOSTSETTING_RESTRICTCHAR_SMALLONLY &&
                i >= BUTTON_MARIO && i < BUTTON_MII_A){
                    DisableButton(&driverButtons[i]);
                }
                if (charRestrict == HOSTSETTING_RESTRICTCHAR_MEDIUMONLY &&
                ((i >= BUTTON_BABY_MARIO && i < BUTTON_MARIO) || (i >= BUTTON_WARIO && i < BUTTON_MII_A))){
                    DisableButton(&driverButtons[i]);
                }
                if (charRestrict == HOSTSETTING_RESTRICTCHAR_LARGEONLY &&
                i >= BUTTON_BABY_MARIO && i < BUTTON_WARIO){
                    DisableButton(&driverButtons[i]);
                }
            }

            // Disables the Miis in Local 2P no matter what.
            if (curSection == SECTION_P2_WIFI ||
            curSection == SECTION_P2_WIFI_FROOM_VS_VOTING ||
            curSection == SECTION_P2_WIFI_FROOM_TEAMVS_VOTING ||
            curSection == SECTION_P2_WIFI_FROOM_BALLOON_VOTING ||
            curSection == SECTION_P2_WIFI_FROOM_COIN_VOTING ||
            (charRestrict == HOSTSETTING_RESTRICTCHAR_SMALLONLY && miiWeight != LIGHTWEIGHT) ||
            (charRestrict == HOSTSETTING_RESTRICTCHAR_MEDIUMONLY && miiWeight != MEDIUMWEIGHT) ||
            (charRestrict == HOSTSETTING_RESTRICTCHAR_LARGEONLY && miiWeight != HEAVYWEIGHT)){
                DisableButton(&driverButtons[BUTTON_MII_A]);
                DisableButton(&driverButtons[BUTTON_MII_B]);
            }
            /*
            // Changes the initially selected button to one that is not disabled.
            if (charRestrict == HOSTSETTING_RESTRICTCHAR_SMALLONLY &&
            ((weight != LIGHTWEIGHT && weight != MIIS) ||
            ((miiWeight != LIGHTWEIGHT || Racedata::sInstance->menusScenario.localPlayerCount > 1) &&
            currentChar >= MII_S_A_MALE))){
                button->HandleDeselect(hudSlotId, -1);
                if (hudSlotId == 0) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(BABY_MARIO));
                else if (hudSlotId == 1) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(BABY_LUIGI));
                newButton->SelectInitialButton(hudSlotId);
                newButton->SetButtonColours(hudSlotId);
                page->OnButtonDriverSelect(newButton, newButton->buttonId, hudSlotId);
            }
            else if (charRestrict == HOSTSETTING_RESTRICTCHAR_MEDIUMONLY &&
            ((weight != MEDIUMWEIGHT && weight != MIIS) ||
            ((miiWeight != MEDIUMWEIGHT || Racedata::sInstance->menusScenario.localPlayerCount > 1) &&
            currentChar >= MII_S_A_MALE))){
                button->HandleDeselect(hudSlotId, -1);
                if (hudSlotId == 0) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(MARIO));
                else if (hudSlotId == 1) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(LUIGI));
                newButton->SelectInitialButton(hudSlotId);
                newButton->SetButtonColours(hudSlotId);
                page->OnButtonDriverSelect(newButton, newButton->buttonId, hudSlotId);
            }
            else if (charRestrict == HOSTSETTING_RESTRICTCHAR_LARGEONLY &&
            ((weight != HEAVYWEIGHT && weight != MIIS) ||
            ((miiWeight != HEAVYWEIGHT || Racedata::sInstance->menusScenario.localPlayerCount > 1) &&
            currentChar >= MII_S_A_MALE))){
                button->HandleDeselect(hudSlotId, -1);
                if (hudSlotId == 0) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(WARIO));
                else if (hudSlotId == 1) newButton = charSelect.GetButtonDriver(static_cast<CharacterId>(WALUIGI));
                newButton->SelectInitialButton(hudSlotId);
                newButton->SetButtonColours(hudSlotId);
                page->OnButtonDriverSelect(newButton, newButton->buttonId, hudSlotId);
            }
            */
        }
    }
    kmCall(0x807e33a8, RestrictCharacterSelection);
} // namespace UI
} // namespace Pulsar