#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <Config.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {

namespace Settings {

u8 Params::radioCount[Params::pageCount] ={
    3, 5, 4, 2, 4, 2 //menu, race, host, KO, OTT, game
    //Add user radio count here

};
u8 Params::scrollerCount[Params::pageCount] ={ 1, 1, 1, 2, 2, 3 }; //menu, race, host, KO, OTT, game

u8 Params::buttonsPerPagePerRow[Params::pageCount][Params::maxRadioCount] = //first row is PulsarSettingsType, 2nd is rowIdx of radio
{
    { 2, 2, 3, 0, 0, 0 }, //Menu
    { 2, 2, 2, 2, 3, 0 }, //Race
    { 3, 4, 2, 2, 0, 0 }, //Host
    { 2, 2, 0, 0, 0, 0 }, //KO
    { 2, 2, 2, 2, 0, 0 }, //OTT
    { 3, 3, 0, 0, 0, 0 }  //Game
};

u8 Params::optionsPerPagePerScroller[Params::pageCount][Params::maxScrollerCount] =
{
    { 5, 7, 0, 0, 0}, //Menu
    { 4, 0, 0, 0, 0}, //Race
    { 11, 0, 0, 0, 0}, //Host
    { 4, 4, 0, 0, 0}, //KO
    { 16, 7, 0, 0, 0}, //OTT
    { 6, 12, 9, 0, 0}  //Game
};

}//namespace Settings
}//namespace Pulsar



