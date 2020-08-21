#pragma once
#include <Windows.h>
#include "VMProtectSDK.h"

#define DRIVER_NAME_S		"\\\\.\\malebolge_drv"
#define GAME_NAME_S			"csgo.exe"
#define SERVICE_NAME_S		"6rycgvhbjpoiougiyvuhkbljb"
#ifndef NDEBUG
#define DRIVER_NAME		DRIVER_NAME_S
#define GAME_NAME		GAME_NAME_S
#define SERVICE_NAME	SERVICE_NAME_S
#else
#define DRIVER_NAME		VMProtectDecryptStringA(DRIVER_NAME_S)
#define GAME_NAME		VMProtectDecryptStringA(GAME_NAME_S)
#define SERVICE_NAME	VMProtectDecryptStringA(SERVICE_NAME_S)
#endif