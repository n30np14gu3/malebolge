#include "globals.h"
#include "driver_defs.h"
#include "ImageLoadCallback.h"

void ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
#ifndef DEBUG
	VMProtectBeginUltra("#ImageLoadCallback");
#endif
	if(wcsstr(FullImageName->Buffer, CLIENT_DLL))
	{
		DbgPrintEx(0, 0, "Client.dll Loaded");
		CLIENT_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
		return;
	}
	if (wcsstr(FullImageName->Buffer, SERVER_DLL))
	{
		DbgPrintEx(0, 0, "Server.dll Loaded");
		SERVER_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
		return;
	}
	if (wcsstr(FullImageName->Buffer, ENGINE_DLL))
	{
		DbgPrintEx(0, 0, "Engine.dll Loaded");
		ENGINE_DLL_BASE = (DWORD32)ImageInfo->ImageBase;
	}
#ifndef DEBUG
	VMProtectEnd();
#endif
}