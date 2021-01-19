#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include <Windows.h>
#include <dinput.h>
#include <tchar.h>
#include <SubAuth.h>
#include <comdef.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#include "../ring0/KernelInterface.h"
#include "../SDK/lazy_importer.hpp"
#include "../SDK/XorStr.hpp"
#include "../themida_sdk/Themida.h"
#include "../SDK/globals.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "Menu.h"
//imgui impl
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//external func
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CleanupDeviceD3D();
void ResetDevice();

static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static HWND g_hwnd = nullptr;
static D3DPRESENT_PARAMETERS* g_d3dpp = nullptr;


#pragma region Menu_class

Menu::Menu(const char* title, void* ptr)
{
	m_iHeight = 130;
	m_iWidth = 450;

	m_d3D = nullptr;
	m_dxDevice = nullptr;
	m_d3dParams = {};

	m_hWindow = nullptr;
	wc = {};
	
	m_szTitle = title;

	m_kDriver = ptr;
}

bool Menu::SetupWindow()
{
	PROTECT_VM_START_HIGH;
	wc = {
	sizeof(WNDCLASSEX),
	CS_CLASSDC,
	WndProc,
	0L,
	0L,
	LI_FN(GetModuleHandleA)(nullptr),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	m_szTitle,
	nullptr
	};
	LI_FN(RegisterClassExA)(&wc);
	m_hWindow = LI_FN(CreateWindowExA)(
		0,
		wc.lpszClassName,
		m_szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		100,
		100,
		m_iWidth,
		m_iHeight,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	if (m_hWindow == nullptr)
		return false;

	LI_FN(SetWindowLongA)(m_hWindow, GWL_STYLE, LI_FN(GetWindowLongA)(m_hWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX);
	LI_FN(SetWindowDisplayAffinity)(m_hWindow, WDA_MONITOR);

	LI_FN(ShowWindow)(m_hWindow, SW_SHOWDEFAULT);

	if (!CreateD3D())
		return false;


	g_hwnd = m_hWindow;
	g_pD3D = m_d3D;
	g_pd3dDevice = m_dxDevice;
	g_d3dpp = &m_d3dParams;

	LI_FN(UpdateWindow)(m_hWindow);

	//Imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.f;
	ImGui::StyleColorsLight();
	ImGui_ImplWin32_Init(m_hWindow);
	ImGui_ImplDX9_Init(g_pd3dDevice);	
	PROTECT_VM_END_HIGH;
	return true;
}

void Menu::Render()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	bool opened = true;
	DWORD dwFlag =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	char buff[256] = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (LI_FN(PeekMessageA).cached()(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			LI_FN(TranslateMessage).cached()(&msg);
			LI_FN(DispatchMessageA).cached()(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(xorstr("gg").crypt_get(), &opened, dwFlag);
		{
			ImGui::SetWindowSize(ImVec2(static_cast<float>(m_iWidth), static_cast<float>(m_iHeight)));
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::InputText(xorstr("Secret path xD").crypt_get(), &buff[0], 256);
			if (ImGui::Button(xorstr("SUCK DICKS").crypt_get(), ImVec2((FLOAT)m_iWidth - 30, 50)))
			{
				PROTECT_MUTATE_START;
				KernelInterface* ring0 = static_cast<KernelInterface*>(m_kDriver);
				std::string str(buff);
				std::wstring wstr(str.begin(), str.end());
				if (ring0->Inject(wstr.c_str()))
					LI_FN(MessageBoxA)(nullptr, xorstr("EZ PZ").crypt_get(), xorstr("OK").crypt_get(), MB_OK);
				PROTECT_MUTATE_END;
			}
		}
		ImGui::End();
		ImGui::EndFrame();

		m_dxDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_dxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_dxDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		m_dxDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);

		if (m_dxDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}
	
	PROTECT_VM_START_LOW;
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
	LI_FN(DestroyWindow)(m_hWindow);
	LI_FN(UnregisterClassA)(wc.lpszClassName, wc.hInstance);
	LI_FN(DeleteFileA)(xorstr("imgui.ini").crypt_get());
	PROTECT_VM_END_LOW;
}

bool Menu::CreateD3D()
{
	PROTECT_VM_START_LOW;
	//Load d3d9.dll
	LI_FN(LoadLibraryA)(xorstr("d3d9.dll").crypt_get());
	if ((m_d3D = LI_FN(Direct3DCreate9)(D3D_SDK_VERSION)) == nullptr)
		return false;

	ZeroMemory(&m_d3dParams, sizeof(m_d3dParams));
	m_d3dParams.Windowed = TRUE;
	m_d3dParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dParams.BackBufferFormat = D3DFMT_UNKNOWN;
	m_d3dParams.EnableAutoDepthStencil = TRUE;
	m_d3dParams.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (m_d3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dParams, &m_dxDevice) < 0)
		return false;
	PROTECT_VM_END_LOW;
	return true;
}

#pragma endregion 


#pragma region NonClassFunc
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp->BackBufferWidth = LOWORD(lParam);
			g_d3dpp->BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		LI_FN(PostQuitMessage)(0);
		return 0;

	default:
		break;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);

}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

#pragma endregion 
