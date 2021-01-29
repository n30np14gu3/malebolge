#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>

#include <ctime>
#include <iostream>


#include "SDK/globals.h"
#include "SDK/lazy_importer.hpp"
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
	const char* file_name = ENCRYPT_STR(".\\settings.ini");
	const char* section_name = ENCRYPT_STR("base_settings");
	
	if (!FileExists(file_name))
		return false;


	dwLocalPlayer = LI_FN(GetPrivateProfileIntA)(section_name, "dwLocalPlayer", 0, file_name);
	dwEntityList = LI_FN(GetPrivateProfileIntA)(section_name, "dwEntityList", 0, file_name);
	mViewMatrix = LI_FN(GetPrivateProfileIntA)(section_name, "mViewMatrix", 0, file_name);

	m_vecOrigin = LI_FN(GetPrivateProfileIntA)(section_name, "m_vecOrigin", 0, file_name);
	m_iHealth = LI_FN(GetPrivateProfileIntA)(section_name, "m_iHealth", 0, file_name);
	m_bDormant = LI_FN(GetPrivateProfileIntA)(section_name, "m_bDormant", 0, file_name);
	m_iTeamNum = LI_FN(GetPrivateProfileIntA)(section_name, "m_iTeamNum", 0, file_name);
	m_BoneMatrix = LI_FN(GetPrivateProfileIntA)(section_name, "m_BoneMatrix", 0, file_name);
	m_aimPunchAngle = LI_FN(GetPrivateProfileIntA)(section_name, "m_aimPunchAngle", 0, file_name);

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
	//HANDLE hMutexHandle = CreateMutexA(nullptr, TRUE, "whN5fKXhAWzpVRpF9rdpmTv9aZH9td");
	//if (GetLastError() != ERROR_ALREADY_EXISTS)
	//	return 0;
	
	srand(static_cast<unsigned>(GetTickCount64()));
	AllocConsole();
	std::string s = "";
	for (size_t i = 0; i < 15; i++)
		s += ALPHABET[rand() % (sizeof(ALPHABET) - 1)];
	SetConsoleTitleA(s.c_str());
	freopen_s(&CON_OUT, ("CONOUT$"), "w", stdout);
	printf_s(("PROJECT Zer0 [FxxF]\n"));
	printf_s(("[Zer0] loading settings...\n"));
	if (!load_offsets())
	{
		printf_s("[Zer0] can't load settings check file!\nExit!\n");
		CloseConsole();
		return 0;
	}

	printf_s("[Zer0] loading driver...\n");
	VM_END;

	KernelInterface ring0;

	VM_START("WinMain_2");
	printf_s("[Zer0] checking driver...\n");

	if (!ring0.NoErrors)
	{
		printf_s("[Zer0] driver not loaded [0x%X]!\nExit!\n", ring0.GetErrorCode());
		CloseConsole();
		return 0;
	}
	printf_s("[Zer0] Getting info...\n");
	while (!ring0.Attach()) {}
	while (!ring0.GetModules()) {}
	printf_s("[Zer0] Completed! Starting...");
	CloseConsole();
	VM_END;
	StartRender(s.c_str(), &ring0, hInstance);
	return 0;
}
