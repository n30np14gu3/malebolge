#include <Windows.h>
#include <Psapi.h>

#include <d3dx9.h>
#include <d3d9.h>

#include "SDK/draw_utils.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/global_defs.h"
#include "SDK/xorstr.hpp"

#include "Hacks/ESP.h"

typedef HRESULT(__stdcall* presentFn)(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);

presentFn OriginalPresent;

RECT DESKTOP_RECT;
DWORD CLIENT_DLL;

D3DXCreateLineFn CreateLine;

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
    	DWORD presentAdr = scanner.FindPattern(xorstr("gameoverlayrenderer.dll").crypt_get(), "\xFF\x15\x00\x00\x00\x00\x8B\xF8\x85\xDB", xorstr("xx????xxxx").crypt_get());
		if (!presentAdr)
			return TRUE;
		presentAdr += 2;

		//get client.dll
		CLIENT_DLL = reinterpret_cast<DWORD>(LI_FN(GetModuleHandleA)(xorstr("client.dll").crypt_get()));

    	//Get desktop rect!
		HWND dDesktop = LI_FN(GetDesktopWindow)();
		LI_FN(GetWindowRect)(dDesktop, &DESKTOP_RECT);
    	
    	//Hook present function
		OriginalPresent = **reinterpret_cast<decltype(OriginalPresent)**>(presentAdr);
		**reinterpret_cast<decltype(hkPresent)***>(presentAdr) = hkPresent;

    	//Cache createLine function (velosiped xD)
		CreateLine = LI_FN(D3DXCreateLine).in(LI_MODULE("d3dx9_43.dll").cached());
    }

    return TRUE;
}
