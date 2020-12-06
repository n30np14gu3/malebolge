#include "globals.h"
#include "driver_defs.h"
#include "functions.h"

//Callbacks
#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"


//BlackBone init
#include "blackbone/BlackBoneDrv.h"
#include "blackbone/Remap.h"
#include "blackbone/Loader.h"
#include "blackbone/Utils.h"

#include <ntstrsafe.h>
#include "DarkTools.h"


PDEVICE_OBJECT pDeviceObj;

UNICODE_STRING DeviceName;
UNICODE_STRING DosName;
UNICODE_STRING DriverName;

//My cheat process
HANDLE PROTECTED_PROCESS;
PEPROCESS PEPROTECTED_PROCESS;

HANDLE GAME_PROCESS;
PEPROCESS PEGAME_PROCESS;

//CS GO MODULES
DWORD32 CLIENT_DLL_BASE;
DWORD32 SERVER_DLL_BASE;
DWORD32 ENGINE_DLL_BASE;
//------------

BOOLEAN DRIVER_INITED;
BOOLEAN BB_INITED;

PDRIVER_OBJECT g_Driver;

extern void spoof();

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

NTSTATUS UnsupportedDispatch(
	_In_ PDEVICE_OBJECT DeviceObject,
	_Inout_ PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
#ifndef DEBUG
	VMProtectBeginUltra("#DriverInit");
#endif
	UNREFERENCED_PARAMETER(pRegistryPath);
	NTSTATUS status;

	g_Driver = pDriverObject;

	RtlSecureZeroMemory(&DeviceName, sizeof(DeviceName));
	RtlInitUnicodeString(&DeviceName, DRIVER_NAME);
	RtlSecureZeroMemory(&DosName, sizeof(DosName));
	RtlInitUnicodeString(&DosName, SYMBOL_NAME);

	pDriverObject->DriverUnload = UnloadDriver;

	status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj);
	IoCreateSymbolicLink(&DosName, &DeviceName);

	if (!NT_SUCCESS(status))
		return status;


	if (pDeviceObj != NULL)
	{
		pDeviceObj->Flags |= DO_DIRECT_IO;
		pDeviceObj->Flags &= ~DO_DEVICE_INITIALIZING;
	}

	for (ULONG t = 0; t < IRP_MJ_MAXIMUM_FUNCTION; t++)
		pDriverObject->MajorFunction[t] = UnsupportedDispatch;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	PsSetLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, FALSE);
	EnableBB();
	return status;
#ifndef DEBUG
	VMProtectEnd();
	EnableCallback();
	spoof();
#endif
}

void EnableBB()
{
	// Get OS Dependant offsets
	InitializeDebuggerBlock();
	NTSTATUS status = BBInitDynamicData(&dynData);
	if (!NT_SUCCESS(status))
	{
		if (status == STATUS_NOT_SUPPORTED)
			DPRINT("BlackBone: %s: Unsupported OS version. Aborting\n", __FUNCTION__);

		return;
	}

	// Initialize some loader structures
	status = BBInitLdrData((PKLDR_DATA_TABLE_ENTRY)g_Driver->DriverSection);
	if (!NT_SUCCESS(status))
	{
		DPRINT("BlackBone: %s: Failed to BBInitLdrData with staus 0x%X\n", __FUNCTION__, status);
		return;
	}

	//
	// Globals init
	//
	InitializeListHead(&g_PhysProcesses);
	RtlInitializeGenericTableAvl(&g_ProcessPageTables, &AvlCompare, &AvlAllocate, &AvlFree, NULL);
	KeInitializeGuardedMutex(&g_globalLock);

	// Setup process termination notifier
	status = PsSetCreateProcessNotifyRoutine(BBProcessNotify, FALSE);
	if (!NT_SUCCESS(status))
	{
		DPRINT("BlackBone: %s: Failed to setup notify routine with staus 0x%X\n", __FUNCTION__, status);
		return;
	}
	DPRINT("Good luck xD");
}


void BBHook()
{
	
}

void BBUnhook()
{
	
}
