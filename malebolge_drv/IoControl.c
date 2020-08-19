#include "globals.h"
#include "functions.h"
#include "driver_io.h"

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTSTATUS NTAPI ExRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
	PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
#ifndef DEBUG
	VMProtectBeginMutation("#IoControl");
	if(VMProtectIsDebuggerPresent(TRUE) || VMProtectIsVirtualMachinePresent())
	{
		ULONG rsp = 0;
		ExRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &rsp);
	}
#endif
	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	PKERNEL_INIT_DATA_REQUEST pInitData;
	
	PKERNEL_WRITE_REQUEST pWriteRequest;
	PKERNEL_READ_REQUEST pReadRequest;
	PKERNEL_WRITE_REQUEST32 pWriteRequest32;
	PKERNEL_READ_REQUEST32 pReadRequest32;
	SIZE_T rwBytes = 0;

	switch(controlCode)
	{
	case IO_INIT_CHEAT_DATA:
		if(!DRIVER_INITED)
		{
			pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
			
			GAME_PROCESS = (HANDLE)pInitData->CsgoId;
			PROTECTED_PROCESS = (HANDLE)pInitData->CheatId;

			pInitData->Result = PsLookupProcessByProcessId(GAME_PROCESS, &PEGAME_PROCESS);
			pInitData->Result |= PsLookupProcessByProcessId(PROTECTED_PROCESS, &PEPROTECTED_PROCESS);
			bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
			DRIVER_INITED = NT_SUCCESS(pInitData->Result);
			if (DRIVER_INITED)
				DbgPrintEx(0, 0, "IO_INIT_CHEAT_DATA");
		}
		break;

	case IO_UPDATE_CHEAT_DATA:
		DRIVER_INITED = FALSE;
		pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		GAME_PROCESS = (HANDLE)pInitData->CsgoId;
		PROTECTED_PROCESS = (HANDLE)pInitData->CheatId;

		pInitData->Result = PsLookupProcessByProcessId(GAME_PROCESS, &PEGAME_PROCESS);
		pInitData->Result |= PsLookupProcessByProcessId(PROTECTED_PROCESS, &PEPROTECTED_PROCESS);
		bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
		DRIVER_INITED = NT_SUCCESS(pInitData->Result);
		if(DRIVER_INITED)
			DbgPrintEx(0, 0, "IO_UPDATE_CHEAT_DATA");
		break;
	case IO_READ_PROCESS_MEMORY:
		if (!DRIVER_INITED)
			break;

		pReadRequest = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if(pReadRequest != NULL)
		{
			pReadRequest->Result = KeReadVirtualMemory(PEGAME_PROCESS, pReadRequest->Address, (pReadRequest->Response), pReadRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST);
		break;

	case IO_WRITE_PROCESS_MEMORY:
		if (!DRIVER_INITED)
			break;
		
		pWriteRequest = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if(pWriteRequest != NULL)
		{
			pWriteRequest->Result = KeWriteVirtualMemory(PEGAME_PROCESS, pWriteRequest->Address, pWriteRequest->Value, pWriteRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_WRITE_REQUEST);
		break;

	case IO_READ_PROCESS_MEMORY_32:
		if (!DRIVER_INITED)
			break;

		pReadRequest32 = (PKERNEL_READ_REQUEST32)Irp->AssociatedIrp.SystemBuffer;
		if (pReadRequest32 != NULL)
		{
			pReadRequest32->Result = KeReadVirtualMemory32(PEGAME_PROCESS, pReadRequest32->Address, (pReadRequest32->Response), pReadRequest32->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST);
		break;

	case IO_WRITE_PROCESS_MEMORY_32:
		if (!DRIVER_INITED)
			break;
		
		pWriteRequest32 = (PKERNEL_WRITE_REQUEST32)Irp->AssociatedIrp.SystemBuffer;
		if (pWriteRequest32 != NULL)
		{
			pWriteRequest32->Result = KeWriteVirtualMemory(PEGAME_PROCESS, pWriteRequest32->Address, pWriteRequest32->Value, pWriteRequest32->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_WRITE_REQUEST);
		break;

		default:break;
	}	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = bytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

#ifndef DEBUG
	VMProtectEnd();
#endif
	return STATUS_SUCCESS;
}

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	return MmCopyVirtualMemory(Process, (PVOID64)SourceAddress, PEPROTECTED_PROCESS, (PVOID64)TargetAddress, Size, KernelMode, ReadedBytes);
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	return MmCopyVirtualMemory(PEPROTECTED_PROCESS, (PVOID64)TargetAddress, Process, (PVOID64)SourceAddress, Size, KernelMode, WritedBytes);
}

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	return MmCopyVirtualMemory(Process, (PVOID)SourceAddress, PEPROTECTED_PROCESS, (PVOID64)TargetAddress, Size, KernelMode, ReadedBytes);
}

NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	return MmCopyVirtualMemory(PEPROTECTED_PROCESS, (PVOID64)TargetAddress, Process, (PVOID)SourceAddress, Size, KernelMode, WritedBytes);
}