#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#include "ring0/KernelInterface.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/XorStr.hpp"
#include "Menu/Menu.h"


FILE* CON_OUT;

static std::string window_title;

__forceinline void CloseConsole()
{
	LI_FN(Sleep)(3000);
	fclose(CON_OUT);
	LI_FN(FreeConsole)();
	PostMessageA(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
	if (!LI_FN(AllocConsole)())
		return FALSE;
	PROTECT_VM_START_LOW;
	freopen_s(&CON_OUT, xorstr("CONOUT$").crypt_get(), "w", stdout);
	printf_s(xorstr("[Malebolge] Ring0 Injector DLC!\n").crypt_get());
	printf_s(xorstr("[Malebolge] created by @shockbyte\n").crypt_get());
	printf_s(xorstr("[Malebolge] Driver interface initialization...\n").crypt_get());
	PROTECT_VM_END_LOW;
	
	KernelInterface ring0;

	PROTECT_VM_START_HIGH;
	printf_s(xorstr("[Malebolge] checking driver...\n").crypt_get());
	if (!ring0.NoErrors)
	{
		printf_s(xorstr("[Malebolge] driver not loaded [%d]!\nExit!\n").crypt_get(), ring0.GetErrorCode());
		CloseConsole();
		return 0;
	}
	
	printf_s(xorstr("[Malebolge] driver loaded! Getting info...\n").crypt_get());
	while (!ring0.Attach()) {}

	printf_s(xorstr("[Malebolge] setup is completed!Let's suck dicks xD\n").crypt_get());
	CloseConsole();
	PROTECT_VM_END_HIGH;

	//window_title = std::string(xorstr("[Malebolge project]").crypt_get());
	window_title = std::string(xorstr(__TIME__).crypt_get());
	Menu menu(window_title.c_str(), &ring0);
	
	PROTECT_VM_START_HIGH;
	if (!menu.SetupWindow())
		return 0;
	menu.Render();
	PROTECT_VM_END_HIGH;
	
	return 0;
}
