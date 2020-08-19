#pragma once
#include "VMProtectSDK.h"

#define DRIVER_NAME_S		"\\\\.\\malebolge_drv"
#define GAME_NAME_S			"Notepad2.exe"
#define CLIENT_DLL_S		"client.dll"

#ifndef NDEBUG
#define DRIVER_NAME	DRIVER_NAME_S
#define GAME_NAME	GAME_NAME_S
#define CLIENT_DLL	CLIENT_DLL_S
#else
#define DRIVER_NAME VMProtectDecryptStringA(DRIVER_NAME_S)
#define GAME_NAME	VMProtectDecryptStringA(GAME_NAME_S)
#define CLIENT_DLL	VMProtectDecryptStringA(CLIENT_DLL_S)
#endif