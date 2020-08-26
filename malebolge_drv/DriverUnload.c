#include "globals.h"
#include "functions.h"
#include "spoof_core.h"

#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"
#include "blackbone/BlackBoneDrv.h"

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
#if DEBUG
	//PRINTF("Driver unloaded!");
#endif

#ifndef DEBUG
	VMProtectBeginUltra("#UnloadDriver");
#endif
	// Unregister notification
	PsSetCreateProcessNotifyRoutine(BBProcessNotify, TRUE);

	// Cleanup physical regions
	BBCleanupProcessPhysList();

	// Cleanup process mapping info
	BBCleanupProcessTable();
	spoof_unload();
	DisableCallback();
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);

#ifndef DEBUG
	VMProtectEnd();
#endif
	return STATUS_SUCCESS;
}
