#pragma once
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/Cheat-Menu-Gun"

#include <vector>
#include <map>

#include <d3d9.h>
#include <dinput.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "patch/injector.hpp"
#include "vkeys.h"


using CallbackTable = std::vector<std::pair<std::string, void(*)()>>;
