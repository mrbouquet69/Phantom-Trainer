using System;
using System.Windows.Forms;
using GTA;
using GTA.Native;
using GTA.Math;
using GTA.UI;

public class PhantomTrainer : Script
{
    bool menuOpen = false;
    int menuIndex = 0;

    bool godMode = false;
    bool noReload = false;
    bool invincibleCar = false;
    bool noBlood = false;

    int lastClearWanted = 0;

    string[] menuItems =
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

    public PhantomTrainer()
    {
        Tick += OnTick;
        KeyDown += OnKeyDown;
    }

    void OnKeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.F5)
        {
            menuOpen = !menuOpen;
        }

        if (!menuOpen) return;

        if (e.KeyCode == Keys.Up)
        {
            menuIndex--;
            if (menuIndex < 0) menuIndex = menuItems.Length - 1;
        }

        if (e.KeyCode == Keys.Down)
        {
            menuIndex++;
            if (menuIndex >= menuItems.Length) menuIndex = 0;
        }

        if (e.KeyCode == Keys.Enter)
        {
            HandleSelection();
        }
    }

    void HandleSelection()
    {
        Ped player = Game.Player.Character;

        switch (menuIndex)
        {
            case 0:
                godMode = !godMode;
                UI.Notify("God Mode: " + (godMode ? "~g~Enabled" : "~r~Disabled"));
                break;

            case 1:
                if (Game.GameTime - lastClearWanted > 2000)
                {
                    Game.Player.WantedLevel = 0;
                    UI.Notify("Wanted level cleared");
                    lastClearWanted = Game.GameTime;
                }
                else
                {
                    UI.Notify("Cooldown active");
                }
                break;

            case 2:
                TeleportToWaypoint();
                break;

            case 3:
                TeleportToObjective();
                break;

            case 4:
                TeleportToPersonalVehicle();
                break;

            case 5:
                noReload = !noReload;
                UI.Notify("No Reload: " + (noReload ? "~g~Enabled" : "~r~Disabled"));
                break;

            case 6:
                invincibleCar = !invincibleCar;
                UI.Notify("Invincible Car: " + (invincibleCar ? "~g~Enabled" : "~r~Disabled"));
                break;

            case 7:
                noBlood = !noBlood;
                UI.Notify("No Blood: " + (noBlood ? "~g~Enabled" : "~r~Disabled"));
                break;
        }
    }

    void OnTick(object sender, EventArgs e)
    {
        Ped player = Game.Player.Character;

        if (godMode)
            player.IsInvincible = true;
        else
            player.IsInvincible = false;

        if (noReload && player.Weapons.Current != null)
        {
            player.Weapons.Current.AmmoInClip = player.Weapons.Current.MaxAmmoInClip;
        }

        if (invincibleCar && player.IsInVehicle())
        {
            Vehicle veh = player.CurrentVehicle;
            veh.IsInvincible = true;
            veh.DirtLevel = 0f;
        }

        if (noBlood)
        {
            Function.Call(Hash.CLEAR_PED_BLOOD_DAMAGE, player);
            player.DirtLevel = 0f;
        }

        if (menuOpen)
            DrawMenu();
    }

    void DrawMenu()
    {
        float startY = 0.3f;

        new UIText("Phantom Trainer", new System.Drawing.Point(20, 20), 0.6f, System.Drawing.Color.White).Draw();

        for (int i = 0; i < menuItems.Length; i++)
        {
            string text = menuItems[i];

            if (i == 0) text += " [" + (godMode ? "ON" : "OFF") + "]";
            if (i == 5) text += " [" + (noReload ? "ON" : "OFF") + "]";
            if (i == 6) text += " [" + (invincibleCar ? "ON" : "OFF") + "]";
            if (i == 7) text += " [" + (noBlood ? "ON" : "OFF") + "]";

            var color = (i == menuIndex) ? System.Drawing.Color.Yellow : System.Drawing.Color.White;

            new UIText(text, new System.Drawing.Point(40, 80 + i * 30), 0.45f, color).Draw();
        }
    }

    void TeleportToWaypoint()
    {
        Blip waypoint = World.WaypointBlip;

        if (waypoint == null)
        {
            UI.Notify("~r~No waypoint set");
            return;
        }

        Vector3 pos = waypoint.Position;
        Game.Player.Character.Position = new Vector3(pos.X, pos.Y, pos.Z + 1);
        UI.Notify("Teleported to waypoint");
    }

    void TeleportToObjective()
    {
        foreach (Blip blip in World.GetAllBlips())
        {
            if (blip.Color == BlipColor.YellowMission)
            {
                Game.Player.Character.Position = blip.Position;
                UI.Notify("Teleported to mission objective");
                return;
            }
        }

        UI.Notify("~r~No mission objective found");
    }

    void TeleportToPersonalVehicle()
    {
        Ped player = Game.Player.Character;

        foreach (Vehicle v in World.GetAllVehicles())
        {
            if (v.Driver == player)
                continue;

            if (v.IsPersistent)
            {
                player.Position = v.Position + new Vector3(2,2,0);
                UI.Notify("Teleported to personal vehicle");
                return;
            }
        }

        UI.Notify("~r~Personal vehicle not found");
    }
}
