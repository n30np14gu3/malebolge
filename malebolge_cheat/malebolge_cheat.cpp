#include <Windows.h>

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE module, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(module, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH)
    {
    	
    }

    return TRUE;
}