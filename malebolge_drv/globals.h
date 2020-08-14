#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
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

//DATA INIT
extern BOOLEAN DRIVER_INITED;

extern ULONG TerminateProcess;

#define PRINTF(x) DbgPrintEx(0, 0, x)