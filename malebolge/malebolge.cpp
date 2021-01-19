#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>
#include "ring0/KernelInterface.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/XorStr.hpp"

KernelInterface ring0;

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    PROTECT_VM_START_HIGH;
    wchar_t fullFilename[MAX_PATH];
    LI_FN(GetFullPathNameW)(L"stub.dll", MAX_PATH, fullFilename, nullptr);
	if(!ring0.NoErrors)
	{
        LI_FN(MessageBoxA)(nullptr,  xorstr("Can't load drv!").crypt_get(), xorstr("GG").crypt_get(), MB_OK);
        return FALSE;
	}
    while (!ring0.Attach()) {}
    bool result = ring0.Inject(fullFilename);
    LI_FN(MessageBoxA)(nullptr, result ? xorstr("OK").crypt_get() : xorstr("Not OK").crypt_get(), xorstr("Pupa").crypt_get(), MB_OK);
	PROTECT_VM_END_HIGH;
    return TRUE;
}