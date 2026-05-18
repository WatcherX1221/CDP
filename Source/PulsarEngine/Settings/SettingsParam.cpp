#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <Config.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {

namespace Settings {

u8 Params::radioCount[Params::pageCount] ={
    2, 4, 5, 4, // PUL  // Misc, Race, Host, OTTKO
    4, 1, 2, 1  // USER // Audio, Item, Physics, Lap
};

u8 Params::scrollerCount[Params::pageCount] ={
    1, 1, 0, 2, // PUL  // Misc, Race, Host, OTTKO
    0, 3, 2, 2  // USER // Audio, Item, Physics, Lap
};

u8 Params::buttonsPerPagePerRow[Params::pageCount][Params::maxRadioCount] = //first row is PulsarSettingsType, 2nd is rowIdx of radio
{
    { 2, 2, 0, 0, 0, 0 }, // Misc
    { 2, 2, 2, 3, 0, 0 }, // Race
    { 3, 2, 2, 2, 2, 0 }, // Host
    { 2, 2, 2, 2, 0, 0 }, // OTTKO
    { 3, 2, 2, 2, 0, 0 }, // Audio
    { 2, 0, 0, 0, 0, 0 }, // Item
    { 3, 3, 0, 0, 0, 0 }, // Physics
    { 3, 0, 0, 0, 0, 0 }  // Lap
};

u8 Params::optionsPerPagePerScroller[Params::pageCount][Params::maxScrollerCount] =
{
    {  5,  7,  0,  0,  0}, // Misc
    {  4,  0,  0,  0,  0}, // Race
    {  0,  0,  0,  0,  0}, // Host
    {  4,  4,  0,  0,  0}, // OTTKO
    {  0,  0,  0,  0,  0}, // Audio
    { 10,  5, 23,  0,  0}, // Item
    { 10, 12,  0,  0,  0}, // Physics
    { 11,  9,  0,  0,  0}  // Lap
};

}//namespace Settings
}//namespace Pulsar



