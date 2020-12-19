#include <Windows.h>
#include "Menu/Menu.h"
#include "ring0/KernelInterface.h"
#include <string>
#include "SDK/http_request/http_request.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    KernelInterface ring0;
	while(!ring0.Attach()) { }
	while(!ring0.GetModules()) { }
    ring0.EnableBB();
    Menu menu(__TIME__, &ring0);
    return 0;
}