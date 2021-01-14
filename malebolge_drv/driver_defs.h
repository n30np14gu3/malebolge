#pragma once
#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

#define DRIVER_NAME_S				L"\\Device\\f4c317"
#define SYMBOL_NAME_S				L"\\DosDevices\\f4c317"
#define MMAP_DRIVER_NAME			L"\\Driver\\f4c317"
#define MM_COPY_VIRTUAL_MEMORY_S	L"MmCopyVirtualMemory"

#define CLIENT_DLL_S	L"\\csgo\\bin\\client.dll"
#define SERVER_DLL_S	L"\\csgo\\bin\\server.dll"
#define ENGINE_DLL_S	L"\\Counter-Strike Global Offensive\\bin\\engine.dll"

#ifndef DEBUG
#define DRIVER_NAME				VMProtectDecryptStringW(DRIVER_NAME_S)
#define SYMBOL_NAME				VMProtectDecryptStringW(SYMBOL_NAME_S)

#define CLIENT_DLL				VMProtectDecryptStringW(CLIENT_DLL_S)
#define SERVER_DLL				VMProtectDecryptStringW(SERVER_DLL_S)
#define ENGINE_DLL				VMProtectDecryptStringW(ENGINE_DLL_S)

#define MM_COPY_VIRTUAL_MEMORY	VMProtectDecryptStringW(L"MmCopyVirtualMemory")

#else

#define DRIVER_NAME				DRIVER_NAME_S
#define SYMBOL_NAME				SYMBOL_NAME_S

#define CLIENT_DLL				CLIENT_DLL_S
#define SERVER_DLL				SERVER_DLL_S
#define ENGINE_DLL				ENGINE_DLL_S
#define MM_COPY_VIRTUAL_MEMORY	MM_COPY_VIRTUAL_MEMORY_S

#endif