#include "globals.h"
#include "functions.h"

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	VM_START("#CloseCall");
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	VM_END;
	return STATUS_SUCCESS;
}