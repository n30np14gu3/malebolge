#include <Windows.h>
#include "SDK/globals.h"
#include "ring0/KernelInterface.h"

KernelInterface ring0;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow)
{
	ring0.Attach();

	DWORD32 dwVal = 0;
	ring0.Read32(0x004FDF00, &dwVal);
	dwVal = 1337;
	ring0.Write32(0x004FDF00, &dwVal);
	return 0;
}