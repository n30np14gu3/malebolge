#include <Windows.h>
#include <string>
#include "../SDK/source_sdk/IClientEntityList.hpp"
#include "../SDK/source_sdk/IVEngineClient.hpp"
#include "../SDK/global_defs.h"
#include "Hooks.h"



typedef void* (*_cdecl createInterfaceFn)(const char*, int*);

void* GetPtr(const char* _module, const char* _interface)
{
    typedef void* (*_cdecl createInterfaceFn)(const char*, int*);
    const HMODULE mod = GetModuleHandle(_module);
	if(mod == nullptr)
	{
        //MessageBox(nullptr, ("Can't find " + std::string(_module)).c_str(), "ERROR", MB_OK);
        ExitProcess(0);
	}
    const createInterfaceFn create = reinterpret_cast<createInterfaceFn>(GetProcAddress(mod, "CreateInterface"));
    if (create == nullptr)
    {
        //MessageBox(nullptr, "Can't find CreateInterface function!", "ERROR", MB_OK);
        ExitProcess(0);
    }


    void* result = create(_interface, nullptr);
    if (result == nullptr)
    {
        //MessageBox(nullptr, ("Can't find " + std::string(_interface) + "!").c_str(), "ERROR", MB_OK);
        ExitProcess(0);
    }

    return result;
}
	
Hooks::Hooks(HMODULE module)
{
#ifdef NDEBUG
    VMProtectBeginUltra("#Hooks::Hooks");
#endif
	
#ifdef NDEBUG
    VMProtectEnd();
#endif
}
