#include "globals.h"
#include "functions.h"
#include "callback.h"

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
#if DEBUG
	PRINTF("Driver unloaded!");
#endif

	DisableCallback();
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	return STATUS_SUCCESS;
}