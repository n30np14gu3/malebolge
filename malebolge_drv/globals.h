#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include "driver_io.h"
#include "VMProtectDDK.h"

extern PDEVICE_OBJECT pDeviceObj;
extern UNICODE_STRING DeviceName;
extern UNICODE_STRING DosName;

//FOR MY PROCESS PROTECT
extern HANDLE PROTECTED_PROCESS;
extern PEPROCESS PEPROTECTED_PROCESS;

//FOR GAME CHEAT
extern HANDLE GAME_PROCESS;
extern PEPROCESS PEGAME_PROCESS;

//CS GO MODULES
extern DWORD32 CLIENT_DLL_BASE;
extern DWORD32 SERVER_DLL_BASE;
extern DWORD32 ENGINE_DLL_BASE;

//DATA INIT
extern BOOLEAN DRIVER_INITED;

extern ULONG TerminateProcess;

#define PRINTF(x) DbgPrintEx(0, 0, x)