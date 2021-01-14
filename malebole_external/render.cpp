#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include <d3d9.h>
#include <d3dx9.h>

#include <dwmapi.h>
#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>


#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <random>

#include "themida_sdk/Themida.h"
#include "ring0/KernelInterface.h"
#include "render.h"

#define dwLocalPlayer	0xD8B2AC
#define dwEntityList	0x4DA2E74
#define mViewMatrix		0x4D94774
#define m_vecOrigin		0x138
#define m_iHealth		0x100
#define m_bDormant		0xED
#define m_iTeamNum		0xF4
#define	m_BoneMatrix	0x26A8

using namespace std;

INT SCREEN_WIDTH = 800;
INT SCREEN_HEIGHT = 600;

HWND OVERLAY_WINDOW;

const MARGINS margin = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

LPDIRECT3DDEVICE9 D3D9_DEVICE;
LPD3DXFONT TEXT_FONT;

RECT DESKTOP_RECT;

KernelInterface* mem;

D3DXVECTOR3 getBonePos(DWORD32 pTargetIn, DWORD32 bone)
{
	DWORD32 matrix = 0;
	mem->Read32<DWORD32>(pTargetIn + m_BoneMatrix, &matrix);
	D3DXVECTOR3 pos{ };
	mem->Read32(matrix + 0x30 * bone + 0xC, &pos.x);
	mem->Read32(matrix + 0x30 * bone + 0x1C, &pos.y);
	mem->Read32(matrix + 0x30 * bone + 0x2C, &pos.z);
	return pos;
}

void initD3D(HWND hWnd)
{
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3D9_DEVICE);
	D3DXCreateFont(D3D9_DEVICE, 16, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &TEXT_FONT);
}


void DrawBox(float x, float y, float width, float height, float px, DWORD color)
{
	ID3DXLine* p_Line;
	D3DXCreateLine(D3D9_DEVICE, &p_Line);
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	p_Line->SetWidth(px);
	p_Line->Draw(points, 5, color);
	p_Line->End();
	p_Line->Release();
}


void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const string& str)
{
	RECT fontPos = { x, y, x + 120, y + 16 };
	g_pFont->DrawText(nullptr, str.c_str(), -1, &fontPos, DT_NOCLIP, color);
}

void Line(float x1, float y1, float x2, float y2, float width, bool antialias, DWORD color)
{
	ID3DXLine* line;
	D3DXCreateLine(D3D9_DEVICE, &line);
	D3DXVECTOR2 linePos[] = { D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2) };
	line->SetWidth(width);
	line->SetAntialias(antialias);
	line->Begin();
	line->Draw(linePos, 2, color);
	line->End();
	line->Release();
}

using matrix = float[4][4];
bool w2s(const D3DXVECTOR3 world, D3DXVECTOR2& screen, const RECT m_Rect, const float flMatrix[4][4])
{
	float w = 0.0f;

	screen.x = flMatrix[0][0] * world.x + flMatrix[0][1] * world.y + flMatrix[0][2] * world.z + flMatrix[0][3];
	screen.y = flMatrix[1][0] * world.x + flMatrix[1][1] * world.y + flMatrix[1][2] * world.z + flMatrix[1][3];
	w = flMatrix[3][0] * world.x + flMatrix[3][1] * world.y + flMatrix[3][2] * world.z + flMatrix[3][3];

	if (w < 0.01f)
		return false;

	const float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	int width = static_cast<int>(m_Rect.right - m_Rect.left);
	int height = static_cast<int>(m_Rect.bottom - m_Rect.top);

	float x = float(width / 2);
	float y = float(height / 2);

	x += float(screen.x * x);
	y -= float(screen.y * y);

	screen.x = x + m_Rect.left;
	screen.y = y + m_Rect.top;

	return true;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		TerminateProcess(GetCurrentProcess(), 0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		TerminateProcess(GetCurrentProcess(), 0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


void render()
{
	D3D9_DEVICE->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
	D3D9_DEVICE->BeginScene();

	float viewMatrix[4][4];
	
	DWORD32 pLocal = 0;
	DWORD32 localHp = 0;
	DWORD32 localTeam = 0;
	
	DWORD32 pEntity = 0;
	DWORD32 entHp = 0;
	DWORD32 entTeam = 0;
	BOOLEAN entDormant = 0;

	D3DXVECTOR3 entPos(0, 0, 0);
	D3DXVECTOR2 entScreen(0, 0);

	D3DXVECTOR3 entHead(0, 0, 0);
	D3DXVECTOR2 entHeadScreen(0, 0);

	mem->Read32(mem->Modules->bClient + dwLocalPlayer, &pLocal);
	if (!pLocal)
		goto end;

	mem->Read32(pLocal + m_iHealth, &pLocal);
	mem->Read32(pLocal + m_iHealth, &localHp);
	mem->Read32(pLocal + m_iTeamNum, &localTeam);
	
	if (!localHp)
		goto end;


	for (int i = 1; i <= 64; i++)
	{
		mem->Read32(mem->Modules->bClient + (i - 1) * 0x10, &pEntity);
		if(!pEntity)
			continue;

		mem->Read32(pEntity + m_iHealth, &entHp);
		mem->Read32(pEntity + m_iTeamNum, &entTeam);
		mem->Read32(pEntity + m_bDormant, &entDormant);
		
		if(!entHp || entTeam == localTeam || entDormant)
			continue;


		mem->Read32(mem->Modules->bClient + mViewMatrix, &viewMatrix);
		mem->Read32(pEntity + m_vecOrigin, &entPos);
		
		if(!w2s(entHead, entHeadScreen, DESKTOP_RECT, viewMatrix))
			continue;

		entHead = getBonePos(pEntity, 8);
		if (!w2s(entHead, entHeadScreen, DESKTOP_RECT, viewMatrix))
			continue;

		FLOAT boxHeight = abs(entHeadScreen.y - entScreen.y);
		FLOAT boxWidth = boxHeight / 2;
		DrawBox(entScreen.x - boxWidth / 2, entHeadScreen.y, boxWidth, boxHeight, 3, D3DCOLOR_ARGB(255, 255, 255, 0));
		Line(SCREEN_WIDTH / 2, SCREEN_HEIGHT, entScreen.x, entScreen.y, 3, true, D3DCOLOR_ARGB(255, 255, 255, 0));
		DrawString(entScreen.x, entScreen.y, D3DCOLOR_ARGB(255, 255, 0, 0), TEXT_FONT, to_string(entHp));
	}
end:
	D3D9_DEVICE->EndScene();
	D3D9_DEVICE->Present(nullptr, nullptr, nullptr, nullptr);
}

void StartRender(
	KernelInterface* ring0,
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
	)
{
	PROTECT_VM_START_HIGH;
	const HWND desktop = GetDesktopWindow();
	if (desktop != nullptr)
	{
		GetWindowRect(desktop, &DESKTOP_RECT);
		SCREEN_WIDTH = DESKTOP_RECT.right - DESKTOP_RECT.left;
		SCREEN_HEIGHT = DESKTOP_RECT.bottom - DESKTOP_RECT.top;
	}
	else
		ExitProcess(0);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = HBRUSH(RGB(0, 0, 0));
	wc.lpszClassName = " ";

	RegisterClassEx(&wc);

	OVERLAY_WINDOW = CreateWindowEx(0, " ", "", WS_EX_TOPMOST | WS_POPUP, DESKTOP_RECT.left, DESKTOP_RECT.top, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	SetWindowLong(OVERLAY_WINDOW, GWL_EXSTYLE, GetWindowLong(OVERLAY_WINDOW, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(OVERLAY_WINDOW, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(OVERLAY_WINDOW, 0, 255, LWA_ALPHA);
	ShowWindow(OVERLAY_WINDOW, nShowCmd);
	SetWindowDisplayAffinity(OVERLAY_WINDOW, WDA_MONITOR);
	initD3D(OVERLAY_WINDOW);
	SetWindowPos(OVERLAY_WINDOW, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	MSG msg;
	PROTECT_VM_END_HIGH;
	while (TRUE)
	{
		

		render();
		
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}