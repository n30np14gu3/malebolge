#include "SDK/globals.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <random>

#include "ring0/KernelInterface.h"
#include "SDK/draw_utils/draw_utils.h"

INT SCREEN_WIDTH = 800;
INT SCREEN_HEIGHT = 600;
HWND OVERLAY_WINDOW;
const MARGINS margin = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
RECT DESKTOP_RECT;

KernelInterface ring0;


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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	while (!ring0.Attach(true)) {}
	//while (!ring0.GetModules()) {}
	ring0.Inject(L"D:\\Osiris.dll");
	MessageBox(nullptr, __TIME__, "GETTING DRIVES", MB_OK);

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
	ShowWindow(OVERLAY_WINDOW, iCmdShow);

	SetWindowDisplayAffinity(OVERLAY_WINDOW, WDA_MONITOR);
	SetWindowText(OVERLAY_WINDOW, __TIME__);
	MSG msg;
	draw_utils utils(OVERLAY_WINDOW, DESKTOP_RECT);
	while (TRUE)
	{
		SetWindowPos(OVERLAY_WINDOW, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		utils.render(&ring0);

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}