#pragma once

//Init cheat code
#define IO_INIT_CHEAT_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_DRIVER_ALIVE     			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_DRIVER_STATUS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Cheat Memory access
#define IO_READ_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3005, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_PROCESS_MEMORY_32		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3006, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY_32		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3007, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Injection Functions (for internal cheats)
#define IO_INJECT_DLL					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3008, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//CS GO ONLY Methods
#define IO_GET_ALL_MODULES				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3009, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


typedef struct _KERNEL_INIT_DATA_REQUEST
{
	DWORD64 CsgoId;
	DWORD64 CheatId;
	NTSTATUS Result;
} KERNEL_INIT_DATA_REQUEST, * PKERNEL_INIT_DATA_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	DWORD64 Address;
	DWORD64 Response;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	DWORD64 Address;
	DWORD64 Value;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

typedef struct _KERNEL_READ_REQUEST32
{
	DWORD32 Address;
	DWORD64 Response;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST32, * PKERNEL_READ_REQUEST32;

typedef struct _KERNEL_WRITE_REQUEST32
{
	DWORD32 Address;
	DWORD64 Value;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST32, * PKERNEL_WRITE_REQUEST32;


typedef struct _KERNEL_GET_MODULE
{
	DWORD64 ImageBase;
	SIZE_T ImageSize;
} KERNEL_GET_MODULE, * PKERNEL_GET_MODULE;

typedef struct _KERNEL_GET_MODULE32
{
	DWORD32 ImageBase;
	SIZE_T ImageSize;
} KERNEL_GET_MODULE32, * PKERNEL_GET_MODULE32;


typedef struct _KERNEL_GET_CSGO_MODULES
{
	DWORD32 bClient;
	DWORD32 bEngine;
	DWORD32 bServer;
	NTSTATUS result;
} KERNEL_GET_CSGO_MODULES, * PKERNEL_GET_CSGO_MODULES;


typedef struct _DRIVER_ALIVE_REQUEST
{
	NTSTATUS status;
} DRIVER_ALIVE_REQUEST, * PDRIVER_ALIVE_REQUEST;

typedef struct _DRIVER_STATUS_REQUEST
{
	NTSTATUS result;
	BOOLEAN bbInited;
	BOOLEAN driverInited;
} DRIVER_STATUS_REQUEST, * PDRIVER_STATUS_REQUEST;


typedef enum _InjectType
{
    IT_Thread,      // CreateThread into LdrLoadDll
    IT_Apc,         // Force user APC into LdrLoadDll
    IT_MMap,        // Manual map
} InjectType;

typedef enum _MmapFlags
{
    KNoFlags = 0x00,    // No flags
    KManualImports = 0x01,    // Manually map import libraries
    KWipeHeader = 0x04,    // Wipe image PE headers
    KHideVAD = 0x10,    // Make image appear as PAGE_NOACESS region
    KRebaseProcess = 0x40,    // If target image is an .exe file, process base address will be replaced with mapped module value
    KNoThreads = 0x80,    // Don't create new threads, use hijacking

    KNoExceptions = 0x01000, // Do not create custom exception handler
    KNoSxS = 0x08000, // Do not apply SxS activation context
    KNoTLS = 0x10000, // Skip TLS initialization and don't execute TLS callbacks
} KMmapFlags;

/// <summary>
/// Input for IO_INJECT_DLL
/// </summary>
typedef struct _INJECT_DLL
{
    InjectType type;                // Type of injection
    wchar_t    FullDllPath[512];    // Fully-qualified path to the target dll
    wchar_t    initArg[512];        // Init routine argument
    ULONG      initRVA;             // Init routine RVA, if 0 - no init routine
    ULONG      pid;                 // Target process ID
    BOOLEAN    wait;                // Wait on injection thread
    BOOLEAN    unlink;              // Unlink module after injection
    BOOLEAN    erasePE;             // Erase PE headers after injection   
    KMmapFlags flags;               // Manual map flags
    ULONGLONG  imageBase;           // Image address in memory to manually map
    ULONG      imageSize;           // Size of memory image
    BOOLEAN    asImage;             // Memory chunk has image layout
} INJECT_DLL, * PINJECT_DLL;