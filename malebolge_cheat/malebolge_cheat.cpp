#include <Windows.h>
#include "Hooks/Hooks.h"

extern "C" BOOL WINAPI _CRT_INIT(HMODULE _module, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE _module, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(_module, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH)
    {
        MessageBox(0, "GG", "SUCK", MB_OK);
    }

    return TRUE;
}
