#include "pch.h"
#include "d3dhook.h"
#include "minhook/MinHook.h"

HRESULT CALLBACK D3dHook::hkGetDeviceState(IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = oGetDeviceState(pThis, cbData, lpvData);

	if (result == DI_OK) 
	{
		if (ImGui::GetIO().MouseDrawCursor) 
		{
			LPDIMOUSESTATE2 mouseState = reinterpret_cast<LPDIMOUSESTATE2>(lpvData);

			// Block camera rotation
			mouseState->lX = 0;
			mouseState->lY = 0;
			mouseState->lZ = 0;

			// Block left & right clicks
			mouseState->rgbButtons[0] = 0;
			mouseState->rgbButtons[1] = 0;
		}
	}
	return result;
}

HRESULT CALLBACK D3dHook::hkGetDeviceData(IDirectInputDevice8* pThis, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) 
{
	HRESULT result = oGetDeviceData(pThis, cbObjectData, rgdod, pdwInOut, dwFlags);

	if (result == DI_OK) 
	{
		if (ImGui::GetIO().MouseDrawCursor) {
			*pdwInOut = 0; //set array size 0
		}
	}
	return result;
}

HRESULT D3dHook::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	return oReset(pDevice, pPresentationParameters);
}

HRESULT CALLBACK D3dHook::hkEndScene(IDirect3DDevice9* pDevice)
{
	ImGuiIO& io = ImGui::GetIO();
	static bool bInit;
	if (bInit)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX9_NewFrame();

		ImGui::NewFrame();

		if (pCallbackFunc != nullptr)
		{
			((void(*)())pCallbackFunc)();
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	else
	{
		bInit = true;

		injector::MakeNOP(0x4E6B1C, 13); // ShowCursor 
		// SetCursorPos
		injector::MakeNOP(0x66E2AD, 8);
		injector::MakeNOP(0x4C0C71, 28);
		injector::WriteMemoryRaw(0x66A030, (void*)"\xC3\x90\x90\x90", 4, true);
		
		ImGui_ImplWin32_Init(GetForegroundWindow());
		ImGui_ImplDX9_Init((IDirect3DDevice9*)pDevice);
		ImGui_ImplWin32_EnableDpiAwareness();

		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", 14.0f);
		io.Fonts->Build();

		ImGuiStyle &style = ImGui::GetStyle();
		style.WindowTitleAlign = ImVec2(0.5, 0.5);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // hide the grip

		oWndProc = (WNDPROC)SetWindowLongPtr(GetForegroundWindow(), GWL_WNDPROC, (LRESULT)WndProc);
	}

	return oEndScene(pDevice);
}

LRESULT D3dHook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool D3dHook::GetD3D9Device(void** pTable, size_t Size)
{
	if (!pTable)
	{
		return false;
	}

	IDirect3D9* pDevice = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pDevice)
	{
		return false;
	}

	IDirect3DDevice9* pDummyDevice = nullptr;

	// Create dummy device
	D3DPRESENT_PARAMETERS d3dpp{};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetForegroundWindow();

	HRESULT dummyDeviceCreated = pDevice->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDeviceCreated != S_OK)
	{
		// may fail in windowed fullscreen mode, trying again with windowed mode
		d3dpp.Windowed = true;
		dummyDeviceCreated = pDevice->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			pDevice->Release();
			return false;
		}
	}

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);
	pDummyDevice->Release();
	pDevice->Release();
	return true;
}

bool D3dHook::GetDinputDevice(void** pTable, size_t Size)
{
	if (!pTable)
	{
		return false;
	}

	IDirectInput8* pDirectInput = NULL;

	// Create dummy device
	if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&pDirectInput, NULL) != DI_OK)
	{
		return false;
	}

	LPDIRECTINPUTDEVICE8  lpdiMouse;
	if (pDirectInput->CreateDevice(GUID_SysMouse, &lpdiMouse, NULL) != DI_OK)
	{
		pDirectInput->Release();
		return false;
	}

	pDirectInput->Release();
	memcpy(pTable, *reinterpret_cast<void***>(lpdiMouse), Size);
	return true;
}

void D3dHook::InjectHook(void* pCallback)
{
	ImGui::CreateContext();

	static void *d3d9Device[119], *dinput8Device[32];
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)) 
		&& GetDinputDevice(dinput8Device, sizeof(dinput8Device))
	)
	{
		pCallbackFunc = pCallback;
		MH_Initialize();
		MH_CreateHook(d3d9Device[16], hkReset, (void**)&oReset);
		MH_CreateHook(d3d9Device[42], hkEndScene, (void**)&oEndScene);
		MH_CreateHook(dinput8Device[9], hkGetDeviceState, (void**)&oGetDeviceState);
		MH_CreateHook(dinput8Device[10], hkGetDeviceData, (void**)&oGetDeviceData);

		MH_EnableHook(MH_ALL_HOOKS);
	}
}

void D3dHook::RemoveHook()
{
	SetWindowLongPtr(GetForegroundWindow(), GWL_WNDPROC, (LRESULT)oWndProc);
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
