#include "SDK/globals.h"
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow)
{
	
#ifndef _DEBUG
	VMProtectBeginUltra("#WinMain");
#endif

#ifndef _DEBUG
	VMProtectEnd();
#endif
	return 0;
}