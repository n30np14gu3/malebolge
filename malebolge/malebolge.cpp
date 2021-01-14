#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>
#include "ring0/KernelInterface.h"

KernelInterface ring0;

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    return TRUE;
}