#pragma once
#include "pch.h"

class MenuWindow
{
private:
	static inline bool bShowMenu;

	static inline bool bGodMode, bUnlimitedAmmo;
	static inline HANDLE handle = GetModuleHandle(NULL);
public:
	MenuWindow() = delete;
	MenuWindow(MenuWindow&) = delete;
	void operator=(MenuWindow const&) = delete;

	// Core
	static void Draw();
	static void Process();

	// Tabs
	static void CheatsTab();
	static void MenuTab();

private:
	static inline CallbackTable header{ {"Cheats", &CheatsTab},   {"Menu", &MenuTab} };
};

