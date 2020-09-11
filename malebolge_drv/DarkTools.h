#pragma once

#define HOOK_SUCCESS(Status) (HOOK_STATUS_SUCCESS == (Status))

typedef enum _HOOK_STATUS
{
	HOOK_STATUS_SUCCESS = 0,
	HOOK_INVALID_PARAMETER

} HOOK_STATUS, * PHOOK_STATUS;

typedef struct _HOOK_DATA
{
	PVOID Address; // Address at which to place the hook
	PVOID HookAddress; // Address of the function to use for the hook
	PVOID OriginalAssemblyAddress; // Address of the original assembly that the hook is going to replace
	BOOLEAN DisableWP; // Disable write protection bit in CR0 or not? Used for writing to system driver's memory
	PDRIVER_DISPATCH ReturnAddress; // Address to which the hook function should return
} HOOK_DATA, * PHOOK_DATA;

NTSTATUS KernelCopy(PVOID Src, PVOID Dst, SIZE_T Size, PSIZE_T Ret);
HOOK_STATUS PlaceDriverInlineHook(PHOOK_DATA HookData);