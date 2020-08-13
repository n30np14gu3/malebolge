#include <Windows.h>
#include "ring0/KernelInterface.h"




int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	KernelInterface i("\\\\.\\malebolge_drv");
	MessageBox(nullptr, "EZ PZ", "EZZZ", MB_OK);
	return 0;
}