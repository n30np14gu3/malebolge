#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>

#include <ctime>
#include <iostream>


#include "SDK/globals.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/XorStr.hpp"
#include "SDK/VmpSdk.h"
#include "ring0/KernelInterface.h"
#include "render.h"


FILE* CON_OUT;


DWORD32 dwLocalPlayer;
DWORD32 dwEntityList;
DWORD32 mViewMatrix;
DWORD32 m_vecOrigin;
DWORD32 m_iHealth;
DWORD32 m_bDormant;
DWORD32 m_iTeamNum;
DWORD32 m_BoneMatrix;
DWORD32 m_aimPunchAngle;

void CloseConsole()
{
	Sleep(3000);
	fclose(CON_OUT);
	FreeConsole();
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}


const char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

bool FileExists(const char* file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = LI_FN(FindFirstFileA)(file, &FindFileData);
	bool found = handle != INVALID_HANDLE_VALUE;
	if (found)
		LI_FN(FindClose)(handle);
	return found;
}

bool load_offsets()
{
	if (!FileExists(xorstr(".\\settings.ini").crypt_get()))
		return false;

	dwLocalPlayer = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("dwLocalPlayer").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	dwEntityList = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("dwEntityList").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	mViewMatrix = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("mViewMatrix").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());

	m_vecOrigin = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_vecOrigin").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	m_iHealth = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_iHealth").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	m_bDormant = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_bDormant").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	m_iTeamNum = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_iTeamNum").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	m_BoneMatrix = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_BoneMatrix").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());
	m_aimPunchAngle = LI_FN(GetPrivateProfileIntA)(xorstr("base_settings").crypt_get(), xorstr("m_aimPunchAngle").crypt_get(), 0, xorstr(".\\settings.ini").crypt_get());

	return true;
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
)
{
	VM_START("WinMain_1");
	srand(GetTickCount());
	AllocConsole();
	std::string s = "";
	for (size_t i = 0; i < 15; i++)
		s += ALPHABET[rand() % (sizeof(ALPHABET) - 1)];
	SetConsoleTitleA(s.c_str());
	freopen_s(&CON_OUT, xorstr("CONOUT$").crypt_get(), "w", stdout);
	printf_s(xorstr("PROJECT R3P [XooX]\n").crypt_get());
	printf_s(xorstr("[R3P] loading settings...\n").crypt_get());
	if (!load_offsets())
	{
		printf_s(xorstr("[R3P] can't load settings check file!\nExit!\n").crypt_get());
		CloseConsole();
		return 0;
	}

	printf_s(xorstr("[R3P] loading driver...\n").crypt_get());
	VM_END;
	
	KernelInterface ring0;

	VM_START("WinMain_2");
	printf_s(xorstr("[R3P] checking driver...\n").crypt_get());
	if (!ring0.NoErrors)
	{
		printf_s(xorstr("[R3P] driver not loaded [%d]!\nExit!\n").crypt_get(), ring0.GetErrorCode());
		CloseConsole();
		return 0;
	}
	printf_s(xorstr("[R3P] Getting info...\n").crypt_get());
	while (!ring0.Attach()) {}
	while (!ring0.GetModules()) {}
	printf_s(xorstr("[R3P] Completed! Starting...").crypt_get());
	CloseConsole();
	VM_END;
	StartRender(s.c_str(), &ring0, hInstance);
	return 0;
}
