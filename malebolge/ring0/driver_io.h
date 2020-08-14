#pragma once

//Init cheat code
#define IO_INIT_CHEAT_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Cheat Protection codes
#define IO_ENABLE_CHEAT_PROTECTION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_DISABLE_CHEAT_PROTECTION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Cheat Memory access
#define IO_ATTACH_TO_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_PROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x02005, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_DETACH_TO_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x02006, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)



typedef struct _KERNEL_INIT_DATA_REQUEST
{
	ULONG CsgoId;
	ULONG CheatId;
	NTSTATUS Result;
} KERNEL_INIT_DATA_REQUEST, * PKERNEL_INIT_DATA_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	ULONG Address;
	ULONG Value;
	ULONG Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONG Address;
	ULONG Response;
	ULONG Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

