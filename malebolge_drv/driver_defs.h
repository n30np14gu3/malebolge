#pragma once
#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

#define DRIVER_NAME_S		L"\\Device\\mobeus_engine"
#define SYMBOL_NAME_S		L"\\DosDevices\\mobeus_engine"
#define MMAP_DRIVER_NAME	L"\\Driver\\asdasdasd"

#define CLIENT_DLL_S	L"\\csgo\\bin\\client.dll"
#define SERVER_DLL_S	L"\\csgo\\bin\\server.dll"
#define ENGINE_DLL_S	L"\\Counter-Strike Global Offensive\\bin\\engine.dll"

#ifndef DEBUG
#define DRIVER_NAME	VMProtectDecryptStringW(DRIVER_NAME_S)
#define SYMBOL_NAME	VMProtectDecryptStringW(SYMBOL_NAME_S)

#define CLIENT_DLL	VMProtectDecryptStringW(CLIENT_DLL_S)
#define SERVER_DLL	VMProtectDecryptStringW(SERVER_DLL_S)
#define ENGINE_DLL	VMProtectDecryptStringW(ENGINE_DLL_S)
#else

#define DRIVER_NAME	DRIVER_NAME_S
#define SYMBOL_NAME	SYMBOL_NAME_S

#define CLIENT_DLL	CLIENT_DLL_S
#define SERVER_DLL	SERVER_DLL_S
#define ENGINE_DLL	ENGINE_DLL_S
#endif