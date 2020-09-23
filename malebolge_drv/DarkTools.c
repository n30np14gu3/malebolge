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