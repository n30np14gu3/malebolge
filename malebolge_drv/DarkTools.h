#pragma once

NTSTATUS KernelCopy(PVOID Src, PVOID Dst, SIZE_T Size, PSIZE_T Ret);

void EnableBB();
