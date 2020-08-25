#include "globals.h"
#include "functions.h"
#include "spoof_core.h"

#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
#if DEBUG
	PRINTF("Driver unloaded!");
#endif

#ifndef DEBUG
	VMProtectBeginUltra("#UnloadDriver");
#endif

	spoof_unload();
	DisableCallback();
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, TRUE);
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);

#ifndef DEBUG
	VMProtectEnd();
#endif
	return STATUS_SUCCESS;
}