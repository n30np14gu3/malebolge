#include <Windows.h>

BOOL APIENTRY DllEntryPoint(HMODULE _module, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        MessageBox(0, "GG", "SUCK", MB_OK);
    }

    return TRUE;
}
