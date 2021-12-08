#include "pch.h"
#include "menuwindow.h"
#include "menuinfo.h"
#include "ui.h"
#include "game/CEntity.h"

void MenuWindow::Draw()
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = bShowMenu;

    if (bShowMenu)
    {
        static bool bInit;

        // Set a proper size
        if (!bInit)
        {
            RECT rect;
            if (GetWindowRect(GetForegroundWindow(), &rect))
            {
                float width = (rect.right - rect.left) / 4.0f;
                float height = (rect.bottom - rect.top) / 2.0f;

                ImGui::SetNextWindowSize(ImVec2(width, height));
            }
            bInit = true;
        }
        
        ImGui::Begin(MENU_TITLE, &bShowMenu, ImGuiWindowFlags_NoCollapse);
        Ui::DrawHeaders(header);
        ImGui::End();
    }
}

void MenuWindow::Process()
{
    // TODO: Implement proper hotkeys
    if ((GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_KEY_M) & 0x8000) != 0)
    {
        bShowMenu = !bShowMenu;
        Sleep(250);
    }
    CEntity* pPlayer = CEntityList::FindPlayer();

    if (bGodMode)
    {
        // TODO: patch memory instead
        pPlayer->m_fHealth = 400;
    }

    if (bUnlimitedAmmo)
    {
        pPlayer->m_nAmmo = 1;
        pPlayer->m_nDynamyteMag = 5;
        pPlayer->m_nRifleMag = 43;
        pPlayer->m_nSniperMag = 50;
        pPlayer->m_nPistolMag = 5;
        pPlayer->m_nBowMag = 9;
    }
}

void MenuWindow::CheatsTab()
{
    ImGui::Columns(2, NULL, false);
    ImGui::Checkbox("God mode", &bGodMode);
    ImGui::NextColumn();
    ImGui::Checkbox("Unlimited ammo", &bUnlimitedAmmo);
    ImGui::Columns(1);

    ImGui::Dummy(ImVec2(0, 10));

    CEntity* pPlayer = CEntityList::FindPlayer();
    ImGui::InputInt("Ammo", (int*)&pPlayer->m_nAmmo);
    if (ImGui::InputFloat("Health", &pPlayer->m_fHealth))
    {
        pPlayer->m_fHealth2 = pPlayer->m_fHealth;
    }
    ImGui::Spacing();
    ImGui::InputInt("Bow Mag", (int*)&pPlayer->m_nBowMag);
    ImGui::InputInt("Dynamite Mag", (int*)&pPlayer->m_nDynamyteMag);
    ImGui::InputInt("Pistol Mag", (int*)&pPlayer->m_nPistolMag);
    ImGui::InputInt("Rifle Mag", (int*)&pPlayer->m_nRifleMag);
    ImGui::InputInt("Sniper Mag", (int*)&pPlayer->m_nSniperMag);
}

void MenuWindow::MenuTab()
{
    if (ImGui::Button("Check update", ImVec2(Ui::GetSize(3))))
    {
        ShellExecute(NULL, "open", "https://github.com/user-grinch/Cheat-Menu/releases/", NULL, NULL, SW_SHOWNORMAL);
    }

    ImGui::SameLine();

    if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(3))))
    {
        ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
    }

    ImGui::SameLine();

    if (ImGui::Button("GitHub repo", ImVec2(Ui::GetSize(3))))
    {
        ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
    }
    ImGui::Spacing();

    if (ImGui::BeginChild("AboutChild"))
    {
        ImGui::Columns(2, nullptr, false);
        ImGui::Text("Author: Grinch_");

        ImGui::Text("Version: %s", MENU_VERSION_NUMBER);

        ImGui::NextColumn();
        ImGui::Text("ImGui: %s", ImGui::GetVersion());
        ImGui::Text("Build: %s", BUILD_NUMBER);

        ImGui::Columns(1);

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::TextWrapped("If you find bugs or have suggestions, let me know on discord.");
        ImGui::Dummy(ImVec2(0, 20));
        ImGui::TextWrapped("Copyright Grinch_ 2021-2022. All rights reserved");

        ImGui::EndChild();
    }
}
