#include "globals.h"
#include "functions.h"
#include "driver_io.h"

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);

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
		}
		break;
	case IO_READ_PROCESS_MEMORY:
		
		if (!DRIVER_INITED)
			break;

		pReadRequest = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if(pReadRequest != NULL)
		{
			pReadRequest->Result = KeReadVirtualMemory(PEGAME_PROCESS, pReadRequest->Address, (pReadRequest->Value), pReadRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST);
		break;

	case IO_WRITE_PROCESS_MEMORY:
		pWriteRequest = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if(pWriteRequest != NULL)
		{
			pWriteRequest->Result = KeWriteVirtualMemory(PEGAME_PROCESS, (PVOID)(&pWriteRequest->Response), (PVOID)pWriteRequest->Address, pWriteRequest->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST);
		break;
		
		default:
		
		break;
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
	PVOID pBuff = ExAllocatePoolWithTag(NonPagedPool, Size, 'trak');
	if (pBuff == NULL)
	{
		return STATUS_ACCESS_DENIED;
	}

	NTSTATUS result = MmCopyVirtualMemory(PEPROTECTED_PROCESS, (PVOID)TargetAddress, PsGetCurrentProcess(),
		pBuff, Size, KernelMode, ReadedBytes);
	
	if(!NT_SUCCESS(result))
	{
		ExFreePoolWithTag(pBuff, 'trak');
		return result;
	}

	MM_COPY_ADDRESS addr =  { (PVOID)TargetAddress };
	result = MmCopyMemory(pBuff, addr, Size, MM_COPY_MEMORY_VIRTUAL, ReadedBytes);
	ExFreePoolWithTag(pBuff, 'trak');
	return result;
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	return MmCopyVirtualMemory(PEPROTECTED_PROCESS, SourceAddress, Process,
		TargetAddress, Size, KernelMode, WritedBytes);
}
