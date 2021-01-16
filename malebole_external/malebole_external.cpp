#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>
#include "themida_sdk/Themida.h"
#include "ring0/KernelInterface.h"
#include "render.h"
#include <iostream>
FILE* CON_OUT;

void CloseConsole()
{
	Sleep(3000);
	fclose(CON_OUT);
	FreeConsole();
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
)
{

	
	KernelInterface ring0;
	PROTECT_VM_START_HIGH;
	AllocConsole();

	freopen_s(&CON_OUT, "CONOUT$", "w", stdout);


	printf_s("[Malebolge] external Ring0 DLC!\n");
	printf_s("[Malebolge] created by @shockbyte\n");
	printf_s("[Malebolge] * DirectX ESP\n");
	printf_s("[Malebolge] checking driver...\n");
	if(!ring0.NoErrors)
	{
		printf_s("[Malebolge] driver not loaded [%d]!\nExit!\n", ring0.GetErrorCode());
		CloseConsole();
		return 0;
	}
	printf_s("[Malebolge] driver loaded! Getting info...\n");
	while(!ring0.Attach()) { }
	while(!ring0.GetModules()) { }
	printf_s("[Malebolge] Ez! Let's go to CS:GO!");
	CloseConsole();
	PROTECT_VM_END_HIGH;
	StartRender(&ring0, hInstance, hPrevInstance, lpCmdLine, nShowCmd);
	return 0;
}