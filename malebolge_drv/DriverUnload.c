#include "globals.h"
#include "functions.h"
#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"
#include "blackbone/BlackBoneDrv.h"
#include "DarkTools.h"

void UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	VM_START("#UnloadDriver");
	DisableBB();
	BBUnhook();
	
	DisableCallback();
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, TRUE);
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);

	DPRINT("Driver unloaded!");
	VM_END;
}

void DisableBB()
{
	if (!BB_INITED)
		return;
	
	// Unregister notification
	PsSetCreateProcessNotifyRoutine(BBProcessNotify, TRUE);

	// Cleanup physical regions
	BBCleanupProcessPhysList();

	// Cleanup process mapping info
	BBCleanupProcessTable();
}
