#include <Windows.h>
#include <string>
#include "../SDK/global_defs.h"
#include "Hooks.h"

Hooks::Hooks(HMODULE module)
{
#ifdef NDEBUG
    VMProtectBeginUltra("#Hooks::Hooks");
#endif
	
#ifdef NDEBUG
    VMProtectEnd();
#endif
}
