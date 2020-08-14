#include "globals.h"
#include "driver_defs.h"
#include "functions.h"
#include "callback.h"


ULONG RANDOM_SEED;

PDEVICE_OBJECT pDeviceObj;

UNICODE_STRING DeviceName;
UNICODE_STRING DosName;

HANDLE PROTECTED_PROCESS;
PEPROCESS PEPROTECTED_PROCESS;

HANDLE GAME_PROCESS;
PEPROCESS PEGAME_PROCESS;

BOOLEAN DRIVER_INITED;

ULONG TerminateProcess;

//Driver ep
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
#ifndef DEBUG
	VMProtectBeginUltra("#DriverEntry");
#endif
	NTSTATUS result;

	RtlIsZeroMemory(&DeviceName, sizeof(DeviceName));
	RtlIsZeroMemory(&DosName, sizeof(DosName));
	
	RtlInitUnicodeString(&DeviceName, DRIVER_NAME);
	RtlInitUnicodeString(&DosName, SYMBOL_NAME);
	
	pDriverObject->DriverUnload = (PDRIVER_UNLOAD)UnloadDriver;
	
	result = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj);
	IoCreateSymbolicLink(&DosName, &DeviceName);
	
	if(!NT_SUCCESS(result))
	{
		#if DEBUG
		PRINTF("Error when device creating!");
		#endif
		return result;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	if(pDeviceObj != NULL)
	{
		pDeviceObj->Flags |= DO_DIRECT_IO;
		pDeviceObj->Flags &= ~DO_DEVICE_INITIALIZING;
	}
	
#ifndef DEBUG
	VMProtectEnd();
	EnableCallback();
#endif
	return STATUS_SUCCESS;
}
