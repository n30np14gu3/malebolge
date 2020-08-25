#pragma once

//Init cheat code
#define IO_INIT_CHEAT_DATA				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_UPDATE_CHEAT_DATA			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Cheat Memory access
#define IO_READ_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_PROCESS_MEMORY_32		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_PROCESS_MEMORY_32		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3005, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//Injection Functions (for internal cheats)
#define IO_INJECT_DLL					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3006, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//CS GO ONLY Methods
#define IO_GET_CLIENT_DLL				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3007, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_SERVER_DLL				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3008, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_ENGINE_DLL				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3009, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_ALL_MODULES				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x300A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


typedef struct _KERNEL_INIT_DATA_REQUEST
{
	DWORD64 CsgoId;
	DWORD64 CheatId;
	NTSTATUS Result;
} KERNEL_INIT_DATA_REQUEST, *PKERNEL_INIT_DATA_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	DWORD64 Address;
	DWORD64 Response;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST, *PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	DWORD64 Address;
	DWORD64 Value;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST, *PKERNEL_WRITE_REQUEST;

typedef struct _KERNEL_READ_REQUEST32
{
	DWORD32 Address;
	DWORD64 Response;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_READ_REQUEST32, *PKERNEL_READ_REQUEST32;

typedef struct _KERNEL_WRITE_REQUEST32
{
	DWORD32 Address;
	DWORD64 Value;
	SIZE_T Size;
	NTSTATUS Result;
} KERNEL_WRITE_REQUEST32, *PKERNEL_WRITE_REQUEST32;


typedef struct _KERNEL_GET_MODULE
{
	DWORD64 ImageBase;
	SIZE_T ImageSize;
} KERNEL_GET_MODULE, *PKERNEL_GET_MODULE;

typedef struct _KERNEL_GET_MODULE32
{
	DWORD32 ImageBase;
	SIZE_T ImageSize;
} KERNEL_GET_MODULE32, *PKERNEL_GET_MODULE32;


typedef struct _KERNEL_GET_CSGO_MODULES
{
	DWORD32 bClient;
	DWORD32 bEngine;
	DWORD32 bServer;
	NTSTATUS result;
} KERNEL_GET_CSGO_MODULES, *PKERNEL_GET_CSGO_MODULES;

/// <summary>
/// Input for IOCTL_BLACKBONE_DISABLE_DEP
/// </summary>
typedef struct _DISABLE_DEP
{
    ULONG   pid;            // Process ID
} DISABLE_DEP, * PDISABLE_DEP;

/// <summary>
/// Policy activation option
/// </summary>
typedef enum _PolicyOpt
{
    Policy_Disable,
    Policy_Enable,
    Policy_Keep,        // Don't change current value
} PolicyOpt;

/// <summary>
/// Input for IOCTL_BLACKBONE_SET_PROTECTION
/// </summary>
typedef struct _SET_PROC_PROTECTION
{
    ULONG pid;              // Process ID
    PolicyOpt protection;   // Process protection
    PolicyOpt dynamicCode;  // DynamiCode policy
    PolicyOpt signature;    // BinarySignature policy
} SET_PROC_PROTECTION, * PSET_PROC_PROTECTION;

/// <summary>
/// Input for IOCTL_BLACKBONE_GRANT_ACCESS
/// </summary>
typedef struct _HANDLE_GRANT_ACCESS
{
    ULONGLONG  handle;      // Handle to modify
    ULONG      pid;         // Process ID
    ULONG      access;      // Access flags to grant
} HANDLE_GRANT_ACCESS, * PHANDLE_GRANT_ACCESS;

/// <summary>
/// Input for IOCTL_BLACKBONE_COPY_MEMORY
/// </summary>
typedef struct _COPY_MEMORY
{
    ULONGLONG localbuf;         // Buffer address
    ULONGLONG targetPtr;        // Target address
    ULONGLONG size;             // Buffer size
    ULONG     pid;              // Target process id
    BOOLEAN   write;            // TRUE if write operation, FALSE if read
} COPY_MEMORY, * PCOPY_MEMORY;

/// <summary>
/// Input for IOCTL_BLACKBONE_ALLOCATE_FREE_MEMORY
/// </summary>
typedef struct _ALLOCATE_FREE_MEMORY
{
    ULONGLONG base;             // Region base address
    ULONGLONG size;             // Region size
    ULONG     pid;              // Target process id
    ULONG     protection;       // Memory protection for allocation
    ULONG     type;             // MEM_RESERVE/MEM_COMMIT/MEM_DECOMMIT/MEM_RELEASE
    BOOLEAN   allocate;         // TRUE if allocation, FALSE is freeing
    BOOLEAN   physical;         // If set to TRUE, physical pages will be directly mapped into UM space
} ALLOCATE_FREE_MEMORY, * PALLOCATE_FREE_MEMORY;

/// <summary>
/// Output for IOCTL_BLACKBONE_ALLOCATE_FREE_MEMORY
/// </summary>
typedef struct _ALLOCATE_FREE_MEMORY_RESULT
{
    ULONGLONG address;          // Address of allocation
    ULONGLONG size;             // Allocated size
} ALLOCATE_FREE_MEMORY_RESULT, * PALLOCATE_FREE_MEMORY_RESULT;

/// <summary>
/// Input for IOCTL_BLACKBONE_PROTECT_MEMORY
/// </summary>
typedef struct _PROTECT_MEMORY
{
    ULONGLONG base;             // Region base address
    ULONGLONG size;             // Region size
    ULONG     pid;              // Target process id
    ULONG     newProtection;    // New protection value
} PROTECT_MEMORY, * PPROTECT_MEMORY;

/// <summary>
/// Input for IOCTL_BLACKBONE_REMAP_MEMORY
/// </summary>
typedef struct _MAP_MEMORY
{
    ULONG   pid;                // Target process id
    wchar_t pipeName[32];       // Hook pipe name
    BOOLEAN mapSections;        // Set to TRUE to map sections
} MAP_MEMORY, * PMAP_MEMORY;

/// <summary>
/// Remapped region info
/// </summary>
typedef struct _MAP_MEMORY_RESULT_ENTRY
{
    ULONGLONG originalPtr;      // Address in target process
    ULONGLONG newPtr;           // Mapped address in host process
    ULONG size;                 // Region size
} MAP_MEMORY_RESULT_ENTRY, * PMAP_MEMORY_RESULT_ENTRY;

/// <summary>
/// Output for IOCTL_BLACKBONE_REMAP_REGION
/// </summary>
typedef struct _MAP_MEMORY_RESULT
{
    ULONGLONG pipeHandle;       // Pipe handle in target process
    ULONGLONG targetPage;       // Address of shared page in target process
    ULONGLONG hostPage;         // Address of shared page in host process

    ULONG count;                // Number of REMAP_MEMORY_RESULT_ENTRY entries

    // List of remapped regions (variable-sized array)
    MAP_MEMORY_RESULT_ENTRY entries[1];
} MAP_MEMORY_RESULT, * PMAP_MEMORY_RESULT;

/// <summary>
/// Input for IOCTL_BLACKBONE_REMAP_REGION
/// </summary>
typedef struct _MAP_MEMORY_REGION
{
    ULONGLONG base;             // Region base address
    ULONG     pid;              // Target process id
    ULONG     size;             // Region size
} MAP_MEMORY_REGION, * PMAP_MEMORY_REGION;

/// <summary>
/// Output for IOCTL_BLACKBONE_REMAP_REGION
/// </summary>
typedef struct _MAP_MEMORY_REGION_RESULT
{
    ULONGLONG originalPtr;      // Address in target process
    ULONGLONG newPtr;           // Mapped address in host process
    ULONGLONG removedPtr;       // Unmapped region base, in case of conflicting region
    ULONG     size;             // Mapped region size
    ULONG     removedSize;      // Unmapped region size
} MAP_MEMORY_REGION_RESULT, * PMAP_MEMORY_REGION_RESULT;

/// <summary>
/// Input for IOCTL_BLACKBONE_UNMAP_MEMORY
/// </summary>
typedef struct _UNMAP_MEMORY
{
    ULONG     pid;              // Target process ID
} UNMAP_MEMORY, * PUNMAP_MEMORY;

/// <summary>
/// Input for IOCTL_BLACKBONE_UNMAP_REGION
/// </summary>
typedef struct _UNMAP_MEMORY_REGION
{
    ULONGLONG base;             // Region base address
    ULONG     pid;              // Target process ID
    ULONG     size;             // Region size
} UNMAP_MEMORY_REGION, * PUNMAP_MEMORY_REGION;


/// <summary>
/// Input for IOCTL_BLACKBONE_HIDE_VAD
/// </summary>
typedef struct _HIDE_VAD
{
    ULONGLONG base;             // Region base address
    ULONGLONG size;             // Region size
    ULONG pid;                  // Target process ID
} HIDE_VAD, * PHIDE_VAD;

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
/// Input for IOCTL_BLACKBONE_INJECT_DLL
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

/// <summary>
/// Input for IOCTL_BLACKBONE_MAP_DRIVER
/// </summary>
typedef struct _MMAP_DRIVER
{
    wchar_t    FullPath[512];    // Fully-qualified path to the driver
} MMAP_DRIVER, * PMMAP_DRIVER;

/// <summary>
/// Input for IOCTL_BLACKBONE_UNLINK_HTABLE
/// </summary>
typedef struct _UNLINK_HTABLE
{
    ULONG      pid;         // Process ID
} UNLINK_HTABLE, * PUNLINK_HTABLE;

/// <summary>
/// Input for IOCTL_BLACKBONE_ENUM_REGIONS
/// </summary>
typedef struct _ENUM_REGIONS
{
    ULONG      pid;         // Process ID
} ENUM_REGIONS, * PENUM_REGIONS;

typedef struct _MEM_REGION
{
    ULONGLONG BaseAddress;
    ULONGLONG AllocationBase;
    ULONG AllocationProtect;
    ULONGLONG RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEM_REGION, * PMEM_REGION;

/// <summary>
/// Output for IOCTL_BLACKBONE_ENUM_REGIONS
/// </summary>
typedef struct _ENUM_REGIONS_RESULT
{
    ULONGLONG  count;                   // Number of records
    MEM_REGION regions[1];              // Found regions, variable-sized
} ENUM_REGIONS_RESULT, * PENUM_REGIONS_RESULT;