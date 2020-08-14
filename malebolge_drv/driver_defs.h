#pragma once
#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

#define DRIVER_NAME_S L"\\Device\\malebolge_drv"
#define SYMBOL_NAME_S L"\\DosDevices\\malebolge_drv"

#ifndef DEBUG
#define DRIVER_NAME VMProtectDecryptStringW(DRIVER_NAME_S)
#define SYMBOL_NAME VMProtectDecryptStringW(SYMBOL_NAME_S)
#else
#define DRIVER_NAME DRIVER_NAME_S
#define SYMBOL_NAME SYMBOL_NAME_S
#endif


extern ULONG RANDOM_SEED;