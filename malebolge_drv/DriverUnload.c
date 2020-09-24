#include "globals.h"
#include "functions.h"
#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"
#include "blackbone/BlackBoneDrv.h"
#include "DarkTools.h"

void UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
#ifndef DEBUG
	VMProtectBeginUltra("#UnloadDriver");
#endif

	DisableBB();
	BBUnhook();
	
	DisableCallback();
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, TRUE);
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);

	DPRINT("Driver unloaded!");
	
#ifndef DEBUG
	VMProtectEnd();
#endif
}

void DisableBB()
{
	// Unregister notification
	PsSetCreateProcessNotifyRoutine(BBProcessNotify, TRUE);

	// Cleanup physical regions
	BBCleanupProcessPhysList();

	// Cleanup process mapping info
	BBCleanupProcessTable();
}
