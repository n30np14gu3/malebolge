#include <Windows.h>
#include <Psapi.h>

#include <d3dx9.h>
#include <d3d9.h>

#include "SDK/draw_utils.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/global_defs.h"

#include "Hacks/ESP.h"

typedef HRESULT(__stdcall* presentFn)(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);

presentFn OriginalPresent;

RECT DESKTOP_RECT;
DWORD CLIENT_DLL;

unsigned char GAMEOVERLAY_DLL[] = { 0x67, 0x60, 0x6F, 0x66, 0x6B, 0x73, 0x63, 0x75, 0x64, 0x68, 0x73, 0x79, 0x69, 0x63, 0x6A, 0x6A, 0x62, 0x74, 0x60, 0x3D, 0x70, 0x79, 0x7A, 0x17 };

unsigned char CLIENT_DLL_S[] = { 0x63, 0x6D, 0x6B, 0x66, 0x6A, 0x71, 0x28, 0x63, 0x64, 0x65, 0x0A };

unsigned char MASK[] = { 0x78, 0x79, 0x3D, 0x3C, 0x3B, 0x3A, 0x7E, 0x7F, 0x70, 0x71, 0x0A };

class SigScan
{
public:
	// For getting information about the executing module
	MODULEINFO GetModuleInfo(const char* szModule)
	{
		MODULEINFO modinfo = { 0 };
		HMODULE hModule = LI_FN(GetModuleHandleA)(szModule);
		if (hModule == 0)
			return modinfo;
		LI_FN(K32GetModuleInformation)(LI_FN(GetCurrentProcess)(), hModule, &modinfo, sizeof(MODULEINFO));
		return modinfo;
	}

	// for finding a signature/pattern in memory of another process
	DWORD FindPattern(const char* _module, const char* pattern, const char* mask)
	{
		MODULEINFO mInfo = GetModuleInfo(_module);
		DWORD base = reinterpret_cast<DWORD>(mInfo.lpBaseOfDll);
		DWORD size = static_cast<DWORD>(mInfo.SizeOfImage);
		DWORD patternLength = static_cast<DWORD>(LI_FN(strlen)(mask));

		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++)
			{
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}
			if (found)
			{
				return base + i;
			}
		}

		return NULL;
	}
};


HRESULT __stdcall hkPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
{
	if (!draw_utils::m_iHeight)
		draw_utils::init_draw_utils(device, DESKTOP_RECT);
	
	render();
	return OriginalPresent(device, src, dest, windowOverride, dirtyRegion);
}

BOOL APIENTRY DllEntryPoint(HMODULE _module, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
		SigScan scanner;

		//Get present function
		decrypt(GAMEOVERLAY_DLL, sizeof(GAMEOVERLAY_DLL));
		decrypt(CLIENT_DLL_S, sizeof(CLIENT_DLL_S));
		decrypt(MASK, sizeof(MASK));
    	DWORD presentAdr = scanner.FindPattern(reinterpret_cast<const char*>(GAMEOVERLAY_DLL), "\xFF\x15\x00\x00\x00\x00\x8B\xF8\x85\xDB", reinterpret_cast<const char*>(MASK));
		if (!presentAdr)
			return TRUE;
		presentAdr += 2;

		//get client.dll
		CLIENT_DLL = reinterpret_cast<DWORD>(LI_FN(GetModuleHandleA)(reinterpret_cast<const char*>(CLIENT_DLL_S)));

    	//Get desktop rect!
		HWND dDesktop = LI_FN(GetDesktopWindow)();
		LI_FN(GetWindowRect)(dDesktop, &DESKTOP_RECT);

		memset(GAMEOVERLAY_DLL, 0, sizeof(GAMEOVERLAY_DLL));
		memset(CLIENT_DLL_S, 0, sizeof(CLIENT_DLL_S));
		memset(MASK, 0, sizeof(MASK));
    	
    	//Hook present function
		OriginalPresent = **reinterpret_cast<decltype(OriginalPresent)**>(presentAdr);
		**reinterpret_cast<decltype(hkPresent)***>(presentAdr) = hkPresent;
    }

    return TRUE;
}
