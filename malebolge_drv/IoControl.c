#include "globals.h"
#include "functions.h"
#include "blackbone/Routines.h"
#include "VMProtectDDK.h"

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);


void InitCheatData(PIRP Irp);
void GetAllModules(PIRP Irp);


NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	
	PKERNEL_WRITE_REQUEST pWriteRequest;
	PKERNEL_READ_REQUEST pReadRequest;
	PKERNEL_WRITE_REQUEST32 pWriteRequest32;
	PKERNEL_READ_REQUEST32 pReadRequest32;
	PDRIVER_STATUS_REQUEST pDriverStatus;
	
	SIZE_T rwBytes = 0;
	switch(controlCode)
	{
	case IO_INIT_CHEAT_DATA:
		InitCheatData(Irp);
		bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
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
			pWriteRequest32->Result = KeWriteVirtualMemory32(PEGAME_PROCESS, pWriteRequest32->Address, pWriteRequest32->Value, pWriteRequest32->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_WRITE_REQUEST);
		break;

	case IO_GET_ALL_MODULES:
		if (!DRIVER_INITED)
			break;
		GetAllModules(Irp);
		bytesIO = sizeof(KERNEL_GET_CSGO_MODULES);
		break;

	case IO_INJECT_DLL:
		VM_START("#IO_INJECT_DLL");
		bytesIO = sizeof(INJECT_DLL);
		
		BBInjectDll((PINJECT_DLL)Irp->AssociatedIrp.SystemBuffer);
		VM_END;
		break;
		default:break;

	case IO_DRIVER_ALIVE:
		bytesIO = sizeof(DRIVER_ALIVE_REQUEST);
		
		((PDRIVER_ALIVE_REQUEST)Irp->AssociatedIrp.SystemBuffer)->status = STATUS_SUCCESS;
		break;

	case IO_GET_DRIVER_STATUS:
		bytesIO = sizeof(DRIVER_STATUS_REQUEST);
		
		pDriverStatus = (PDRIVER_STATUS_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		pDriverStatus->result = STATUS_SUCCESS;
		pDriverStatus->driverInited = DRIVER_INITED;
		pDriverStatus->bbInited = BB_INITED;
		break;
	}	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = bytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	return MmCopyVirtualMemory(
		Process, (PVOID64)SourceAddress, 
		PEPROTECTED_PROCESS, 
		(PVOID64)TargetAddress, Size, 
		KernelMode, 
		ReadedBytes);
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, DWORD64 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	return MmCopyVirtualMemory(
		PEPROTECTED_PROCESS, 
		(PVOID64)TargetAddress, 
		Process, 
		(PVOID64)SourceAddress, 
		Size, 
		KernelMode, 
		WritedBytes);
}

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	return MmCopyVirtualMemory(
		Process,
		(PVOID)SourceAddress,
		PEPROTECTED_PROCESS, 
		(PVOID64)TargetAddress,
		Size, 
		KernelMode,
		ReadedBytes);
}

NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	return MmCopyVirtualMemory(
		PEPROTECTED_PROCESS,
		(PVOID64)TargetAddress,
		Process,
		(PVOID)SourceAddress, 
		Size, 
		KernelMode, 
		WritedBytes);
}


void InitCheatData(PIRP Irp)
{
	VM_START("#InitCheatData");
	PKERNEL_INIT_DATA_REQUEST pInitData;
	pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
	if(!DRIVER_INITED)
	{
		GAME_PROCESS = (HANDLE)pInitData->CsgoId;
		PROTECTED_PROCESS = (HANDLE)pInitData->CheatId;
		pInitData->Result = PsLookupProcessByProcessId(GAME_PROCESS, &PEGAME_PROCESS);
		pInitData->Result |= PsLookupProcessByProcessId(PROTECTED_PROCESS, &PEPROTECTED_PROCESS);
		DRIVER_INITED = NT_SUCCESS(pInitData->Result);
		if(DRIVER_INITED || BB_INITED)
		{
#ifndef DEBUG
			//Protect cheat process
			SET_PROC_PROTECTION prot;
			prot.protection = Policy_Enable;
			prot.dynamicCode = Policy_Enable;
			prot.signature = Policy_Enable;
			prot.pid = (ULONG)PROTECTED_PROCESS;
			BBSetProtection(&prot);
#endif
			
		}
	}
	else
	{
		pInitData->Result = STATUS_SUCCESS;
	}

	VM_END;
}

void GetAllModules(PIRP Irp)
{
	VM_START("#GetAllModules");
	PKERNEL_GET_CSGO_MODULES pModules = (PKERNEL_GET_CSGO_MODULES)Irp->AssociatedIrp.SystemBuffer;
	if (pModules != NULL)
	{
		if (CLIENT_DLL_BASE == 0 || ENGINE_DLL_BASE == 0 || SERVER_DLL_BASE == 0)
			pModules->result = STATUS_ACCESS_DENIED;
		else
		{
			pModules->bClient = CLIENT_DLL_BASE;
			pModules->bEngine = ENGINE_DLL_BASE;
			pModules->bServer = SERVER_DLL_BASE;
			pModules->result = STATUS_SUCCESS;
		}
	}
	VM_END;
}