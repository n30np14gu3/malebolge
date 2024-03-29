#include "globals.h"
#include "driver_defs.h"
#include "ImageLoadCallback.h"

#ifdef DBG
#define DPRINT(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, __VA_ARGS__)
#else
#define DPRINT(...)
#endif

void ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	VM_START("#ImageLoadCallback");
	if(wcsstr(FullImageName->Buffer, CLIENT_DLL))
	{
		CLIENT_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
		DPRINT("Load cldll 0x%X", CLIENT_DLL_BASE);
		return;
	}
	if (wcsstr(FullImageName->Buffer, SERVER_DLL))
	{
		SERVER_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
		DPRINT("Load sedll 0x%X", SERVER_DLL_BASE);
		return;
	}
	if (wcsstr(FullImageName->Buffer, ENGINE_DLL))
	{
		ENGINE_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
		DPRINT("Load endll 0x%X", ENGINE_DLL_BASE);
	}
	VM_END;
}