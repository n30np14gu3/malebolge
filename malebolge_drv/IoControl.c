#include "globals.h"
#include "functions.h"
#include "driver_io.h"

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS result = STATUS_SUCCESS;
	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	PKERNEL_INIT_DATA_REQUEST pInitData;
	switch(controlCode)
	{
	case IO_INIT_CHEAT_DATA:
		pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PROTECTED_PROCESS = (HANDLE)pInitData->CheatId;
		GAME_PROCESS = (HANDLE)pInitData->CsgoId;
		result = PsLookupProcessByProcessId(GAME_PROCESS, &PEGAME_PROCESS);
		pInitData->Result = result;
		bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
	case IO_READ_PROCESS_MEMORY:
		
		break;

		default:
		
		break;
	}
	
	Irp->IoStatus.Status = result;
	Irp->IoStatus.Information = bytesIO;
	
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return result;
}