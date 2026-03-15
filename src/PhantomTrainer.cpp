#include "script.h"
#include "keyboard.h"
#include <string>

bool menuOpen = false;
int menuIndex = 0;

bool godMode = false;
bool noReload = false;
bool invincibleCar = false;
bool noBlood = false;

DWORD lastWantedClear = 0;

const char* menuItems[] =
{
    "God Mode",
    "Clear Wanted",
    "TP To Waypoint",
    "TP To Mission Objective",
    "TP To Personal Car",
    "No Reload",
    "Invincible Car",
    "No Blood"
};

int menuSize = 8;

void notify(const char* text)
{
    UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(text);
    UI::_DRAW_NOTIFICATION(false, false);
}

void drawMenu()
{
    float x = 0.02f;
    float y = 0.30f;

    UI::SET_TEXT_FONT(0);
    UI::SET_TEXT_SCALE(0.35f, 0.35f);
    UI::SET_TEXT_COLOUR(255,255,255,255);

    UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Phantom Trainer");
    UI::END_TEXT_COMMAND_DISPLAY_TEXT(x,y-0.05f);

    for (int i = 0; i < menuSize; i++)
    {
        if (i == menuIndex)
            UI::SET_TEXT_COLOUR(255,255,0,255);
        else
            UI::SET_TEXT_COLOUR(255,255,255,255);

        std::string item = menuItems[i];

        if (i == 0) item += godMode ? " [ON]" : " [OFF]";
        if (i == 5) item += noReload ? " [ON]" : " [OFF]";
        if (i == 6) item += invincibleCar ? " [ON]" : " [OFF]";
        if (i == 7) item += noBlood ? " [ON]" : " [OFF]";

        UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(item.c_str());
        UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y + (i * 0.03f));
    }
}

void tpWaypoint()
{
    int blip = UI::GET_FIRST_BLIP_INFO_ID(8);

    if (!UI::DOES_BLIP_EXIST(blip))
    {
        notify("~r~No waypoint set");
        return;
    }

    Vector3 coords = UI::GET_BLIP_INFO_ID_COORD(blip);

    PED player = PLAYER::PLAYER_PED_ID();
    ENTITY::SET_ENTITY_COORDS(player, coords.x, coords.y, coords.z + 1, true, false, false, true);

    notify("Teleported to waypoint");
}

void tpMission()
{
    int blip = UI::GET_FIRST_BLIP_INFO_ID(1);

    if (!UI::DOES_BLIP_EXIST(blip))
    {
        notify("~r~No mission objective");
        return;
    }

    Vector3 coords = UI::GET_BLIP_INFO_ID_COORD(blip);

    PED player = PLAYER::PLAYER_PED_ID();
    ENTITY::SET_ENTITY_COORDS(player, coords.x, coords.y, coords.z + 1, true, false, false, true);

    notify("Teleported to objective");
}

void tpPersonalCar()
{
    PED player = PLAYER::PLAYER_PED_ID();

    if (PED::IS_PED_IN_ANY_VEHICLE(player,false))
    {
        Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(player,false);
        Vector3 pos = ENTITY::GET_ENTITY_COORDS(veh,true);

        ENTITY::SET_ENTITY_COORDS(player,pos.x+2,pos.y,pos.z,true,false,false,true);

        notify("Teleported to personal vehicle");
    }
    else
    {
        notify("~r~No personal vehicle found");
    }
}

void handleSelection()
{
    PED player = PLAYER::PLAYER_PED_ID();

    switch(menuIndex)
    {
        case 0:
            godMode = !godMode;
            notify(godMode ? "God Mode Enabled" : "God Mode Disabled");
            break;

        case 1:
        {
            DWORD time = GetTickCount();

            if (time - lastWantedClear > 2000)
            {
                PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());
                notify("Wanted Level Cleared");
                lastWantedClear = time;
            }
            else
            {
                notify("Cooldown active");
            }
            break;
        }

        case 2:
            tpWaypoint();
            break;

        case 3:
            tpMission();
            break;

        case 4:
            tpPersonalCar();
            break;

        case 5:
            noReload = !noReload;
            notify(noReload ? "No Reload Enabled" : "No Reload Disabled");
            break;

        case 6:
            invincibleCar = !invincibleCar;
            notify(invincibleCar ? "Invincible Car Enabled" : "Invincible Car Disabled");
            break;

        case 7:
            noBlood = !noBlood;
            notify(noBlood ? "No Blood Enabled" : "No Blood Disabled");
            break;
    }
}

void main()
{
    while (true)
    {
        PED player = PLAYER::PLAYER_PED_ID();

        if (IsKeyJustUp(VK_F5))
            menuOpen = !menuOpen;

        if (menuOpen)
        {
            drawMenu();

            if (IsKeyJustUp(VK_UP))
            {
                menuIndex--;
                if (menuIndex < 0) menuIndex = menuSize-1;
            }

            if (IsKeyJustUp(VK_DOWN))
            {
                menuIndex++;
                if (menuIndex >= menuSize) menuIndex = 0;
            }

            if (IsKeyJustUp(VK_RETURN))
                handleSelection();
        }

        if (godMode)
            ENTITY::SET_ENTITY_INVINCIBLE(player,true);

        if (noReload)
        {
            Hash weapon;
            WEAPON::GET_CURRENT_PED_WEAPON(player,&weapon,true);
            WEAPON::SET_PED_AMMO(player,weapon,9999);
        }

        if (invincibleCar && PED::IS_PED_IN_ANY_VEHICLE(player,false))
        {
            Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(player,false);
            ENTITY::SET_ENTITY_INVINCIBLE(veh,true);
            VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh,0.0f);
        }

        if (noBlood)
        {
            PED::CLEAR_PED_BLOOD_DAMAGE(player);
        }

        WAIT(0);
    }
}

void ScriptMain()
{
    main();
}
