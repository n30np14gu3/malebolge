#pragma once
#include <Windows.h>
#include "VMProtectSDK.h"

#define DRIVER_NAME_S		"\\\\.\\malebolge_drv"
#define GAME_NAME_S			"csgo.exe"
#define SERVICE_NAME_S		"6rycgvhbjpoiougiyvuhkbljb"

#define	dwLocalPlayer		0xD3FC5C
#define	dwEntityList		0x4D5450C
#define	mViewMatrix			0x4D45E54
#define	m_vecOrigin			0x138
#define	m_iHealth			0x100
#define	m_bDormant			0xED
#define	m_iTeamNum			0xF4
#define	m_BoneMatrix		0x26A8
#define m_aimPunchAngle		0x302C

#ifndef NDEBUG
#define DRIVER_NAME		DRIVER_NAME_S
#define GAME_NAME		GAME_NAME_S
#define SERVICE_NAME	SERVICE_NAME_S
#else
#define DRIVER_NAME		VMProtectDecryptStringA(DRIVER_NAME_S)
#define GAME_NAME		VMProtectDecryptStringA(GAME_NAME_S)
#define SERVICE_NAME	VMProtectDecryptStringA(SERVICE_NAME_S)
#endif
