#include <ntifs.h>
#include "globals.h"
#include "utils.h"

NTSTATUS TerminatingProcess(HANDLE targetPid)
{
	if (targetPid == PROTECTED_PROCESS)
	{
		PROTECTED_PROCESS = 0;
	}

	NTSTATUS NtRet;
	PEPROCESS PeProc = { 0 };
	NtRet = PsLookupProcessByProcessId(targetPid, &PeProc);
	if (NtRet != (NTSTATUS)0x00000000L)
	{
		return NtRet;
	}
	HANDLE ProcessHandle;
	NtRet = ObOpenObjectByPointer(PeProc, 0, NULL, 25, *PsProcessType, KernelMode, &ProcessHandle);
	if (NtRet != (NTSTATUS)0x00000000L)
	{
		return NtRet;
	}
	ZwTerminateProcess(ProcessHandle, 0);
	ZwClose(ProcessHandle);
	return NtRet;
}
