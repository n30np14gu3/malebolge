#include "globals.h"
#include "functions.h"

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
#ifndef DEBUG
	VMProtectBeginVirtualization("#CloseCall");
#endif

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
#ifndef DEBUG
	VMProtectEnd();
#endif
	return STATUS_SUCCESS;
}