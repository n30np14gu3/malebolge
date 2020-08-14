#include <Windows.h>
#include "SDK/VMProtectSDK.h"
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
#ifndef DEBUG
	VMProtectBeginUltra("#WinMain");
#endif

#ifndef DEBUG
	VMProtectEnd();
#endif
	return 0;
}