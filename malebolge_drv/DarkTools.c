#include "globals.h"
#include "DarkTools.h"

typedef unsigned char BYTE;

NTSTATUS KernelCopy(PVOID Src, PVOID Dst, SIZE_T Size, PSIZE_T Ret)
{
	NTSTATUS result;
	MM_COPY_ADDRESS adr = { Src };
	
	_disable();
	__writecr0(__readcr0() & (~(1 << 16)));
	result = MmCopyMemory(Dst, adr, Size, MM_COPY_MEMORY_VIRTUAL, Ret);
	__writecr0(__readcr0() | (1 << 16));
	_enable();

	return result;
}

BYTE PUSH_RAX_ADR[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
BYTE JMP_RAX_RET[] = { 0xFF, 0xE0 };
BYTE FUNC_BACKUP[12] = { 0x0 };

void HookFunction(PVOID Func, PVOID Hook, unsigned char* backup)
{
	SIZE_T ret = 0;
	*((PDWORD64)&PUSH_RAX_ADR[2]) = _byteswap_uint64((DWORD64)Hook);
	
	memcpy_s(FUNC_BACKUP, 12, Func, 12);
	memcpy_s(backup, 12, FUNC_BACKUP, 12);
	
	KernelCopy(PUSH_RAX_ADR, Func, 10, &ret);
	KernelCopy(JMP_RAX_RET, (PVOID)((DWORD64)Func + 10), 2, &ret);
}


/// <summary>
/// Place an inline hook on given address in any driver
/// </summary>
/// <param name="HookData">Pointer to a HOOK_DATA structure</param>
/// <returns>Hook status code</returns>
HOOK_STATUS PlaceDriverInlineHook(PHOOK_DATA HookData)
{
	/*
		mov rax, 0h
		jmp rax
		mov rax, 0h
		call rax
	*/
	BYTE shellcode[24] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0, 0x48, 0xB8, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD0 };

	// Check if any of the given addresses are invalid
	if (!MmIsAddressValid(HookData->Address) || !MmIsAddressValid(HookData->HookAddress) || !MmIsAddressValid(HookData->OriginalAssemblyAddress))
		return HOOK_INVALID_PARAMETER;

	// Copy the addresses into the shellcode
	RtlCopyMemory(shellcode + 0x02, &HookData->HookAddress, sizeof(HookData->HookAddress));
	RtlCopyMemory(shellcode + 0x0E, &HookData->OriginalAssemblyAddress, sizeof(HookData->OriginalAssemblyAddress));

	if (HookData->DisableWP)
	{
		_disable();
		__writecr0(__readcr0() & (~(1 << 16)));
	}

	// Copy the shellcode to the destination
	RtlCopyMemory(HookData->Address, &shellcode, sizeof(shellcode));

	if (HookData->DisableWP)
	{
		__writecr0(__readcr0() | (1 << 16));
		_enable();
	}

	HookData->ReturnAddress = (PDRIVER_DISPATCH)((DWORD64)HookData->Address + 0xC);

	return HOOK_STATUS_SUCCESS;
}