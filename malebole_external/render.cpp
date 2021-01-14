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
#include "draw_utils.h"

#define dwLocalPlayer	0xD8B2AC
#define dwEntityList	0x4DA2E74
#define mViewMatrix		0x4D94774
#define m_vecOrigin		0x138
#define m_iHealth		0x100
#define m_bDormant		0xED
#define m_iTeamNum		0xF4
#define	m_BoneMatrix	0x26A8
#define m_aimPunchAngle 0x302C

using namespace std;

HWND OVERLAY_WINDOW;

INT SCREEN_WIDTH = 800;
INT SCREEN_HEIGHT = 600;

const MARGINS margin = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
RECT DESKTOP_RECT;

D3DXVECTOR3 getBonePos(DWORD32 pTargetIn, int bone, KernelInterface* reader)
{
	DWORD32 pMatrix = 0;
	reader->Read32(pTargetIn + m_BoneMatrix, &pMatrix);
	if (!pMatrix)
		return D3DXVECTOR3(0, 0, 0);

	FLOAT px;
	FLOAT py;
	FLOAT pz;
	reader->Read32(pMatrix + 0x30 * bone + 0xC, &px);
	reader->Read32(pMatrix + 0x30 * bone + 0x1C, &py);
	reader->Read32(pMatrix + 0x30 * bone + 0x2C, &pz);

	return D3DXVECTOR3(px, py, pz);

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


DWORD32 pLocal;
DWORD32 localHp;
DWORD32 localTeam;
DWORD32 localCrosshairId;

DWORD32 pEntity;
DWORD32 entHp;
DWORD32 entTeam;
BOOL entDormant;

D3DXVECTOR3 entPos(0, 0, 0);
D3DXVECTOR2 entScreen(0, 0);

D3DXVECTOR3 entHead(0, 0, 0);
D3DXVECTOR2 entHeadScreen(0, 0);

D3DXVECTOR3 vecPunch(0, 0, 0);
float viewMatrix[4][4];

void draw_utils::hackProc(void* ptr)
{
	KernelInterface* ring0 = static_cast<KernelInterface*>(ptr);

	ring0->Read32(ring0->Modules->bClient + dwLocalPlayer, &pLocal);
	if (!pLocal)
		return;

	ring0->Read32(pLocal + m_iTeamNum, &localTeam);
	if (localTeam != 2 && localTeam != 3)
		return;

	ring0->Read32(pLocal + m_aimPunchAngle, &vecPunch);
	crosshair(10,
		D3DXVECTOR2(
			static_cast<float>(m_iWidth) / 2.f - (static_cast<float>(m_iWidth) / 90 * vecPunch.y),
			static_cast<float>(m_iHeight / 2.f) + (static_cast<float>(m_iHeight) / 90 * vecPunch.x)
		),
		D3DCOLOR_ARGB(255, 0, 255, 0));

	for (DWORD32 i = 1; i < 64; i++)
	{
		ring0->Read32(ring0->Modules->bClient + dwEntityList + (i - 1) * 0x10, &pEntity);
		if (!pEntity)
			continue;

		ring0->Read32(pEntity + m_iHealth, &entHp);
		if (!entHp)
			continue;

		ring0->Read32(pEntity + m_iTeamNum, &entTeam);
		if (entTeam == localTeam)
			continue;


		ring0->Read32(pEntity + m_bDormant, &entDormant);
		if (entDormant)
			continue;

		ring0->Read32(ring0->Modules->bClient + mViewMatrix, &viewMatrix);
		ring0->Read32(pEntity + m_vecOrigin, &entPos);
		if (!w2s(entPos, entScreen, viewMatrix))
			continue;

		entHead = getBonePos(pEntity, 8, ring0);
		if (entHead.x == 0 && entHead.y == 0 && entHead.z == 0.f)
			continue;

		if (!w2s(entHead, entHeadScreen, viewMatrix))
			continue;

		FLOAT boxHeight = abs(entHeadScreen.y - entScreen.y);
		FLOAT boxWidth = boxHeight / 2;
		healthBox(entScreen.x - boxWidth / 2, entHeadScreen.y, boxWidth, boxHeight, 2.f, entHp, D3DCOLOR_ARGB(255, 255, 0, 0), D3DCOLOR_ARGB(255, 0, 255, 0));
		string(D3DXVECTOR2(entScreen.x, entScreen.y), std::to_string(entHp), D3DCOLOR_ARGB(255, 255, 255, 0));
	}
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
	HWND desktop = GetDesktopWindow();
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
	SetWindowPos(OVERLAY_WINDOW, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	MSG msg;
	draw_utils render(OVERLAY_WINDOW, DESKTOP_RECT);
	PROTECT_VM_END_HIGH;
	while (TRUE)
	{


		render.render((void*)ring0);

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}