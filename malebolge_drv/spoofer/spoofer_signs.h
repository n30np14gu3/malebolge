#pragma once
#include "../VMProtectDDK.h"

//Disk spoof signs
#define STORPORT_S								"storport.sys"

#define RAID_UNIT_REGISTER_INTERFACE_SIGN_S		"\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x50"
#define RAID_UNIT_REGISTER_INTERFACE_MASK_S		"xxxx?xxxxxxx"

#define DEVICE_RAID_PORT_S						L"\\Device\\RaidPort%d"

#define DISK_SYS_S								"disk.sys"
#define DISK_DISABLE_PREDICTION_SIGN_S			"\x4C\x8B\xDC\x49\x89\x5B\x10\x49\x89\x7B\x18\x55\x49\x8D\x6B\xA1\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x4F"
#define DISK_DISABLE_PREDICTION_MASK_S			"xxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx"
#define DRIVER_DISK_S							L"\\Driver\\Disk"
//---------------------------------------

#define NTOSKRNL_S								"ntoskrnl.exe"
#define SMBIOS_SIGN1_S							"\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x8B\x15"
#define SMBIOS_MASK1_S							"xxx????xxxx?xx"

#define SMBIOS_SIGN2_S							"\x8B\x1D\x00\x00\x00\x00\x48\x8B\xD0\x44\x8B\xC3\x48\x8B\xCD\xE8\x00\x00\x00\x00\x8B\xD3\x48\x8B"
#define SMBIOS_MASK2_S							"xx????xxxxxxxxxx????xxxx"


#ifdef DBG
#define STORPORT							STORPORT_S

#define RAID_UNIT_REGISTER_INTERFACE_SIGN	RAID_UNIT_REGISTER_INTERFACE_SIGN_S
#define RAID_UNIT_REGISTER_INTERFACE_MASK	RAID_UNIT_REGISTER_INTERFACE_MASK_S

#define DEVICE_RAID_PORT					DEVICE_RAID_PORT_S

#define DISK_SYS							DISK_SYS_S
#define DISK_DISABLE_PREDICTION_SIGN		DISK_DISABLE_PREDICTION_SIGN_S
#define DISK_DISABLE_PREDICTION_MASK		DISK_DISABLE_PREDICTION_MASK_S
#define DRIVER_DISK							DRIVER_DISK_S

#define NTOSKRNL							NTOSKRNL_S

#define SMBIOS_SIGN1						SMBIOS_SIGN1_S
#define SMBIOS_MASK1						SMBIOS_MASK1_S

#define SMBIOS_SIGN2						SMBIOS_SIGN2_S
#define SMBIOS_MASK2						SMBIOS_MASK2_S
#else
#define STORPORT							VMProtectDecryptStringA(STORPORT_S)

#define RAID_UNIT_REGISTER_INTERFACE_SIGN	RAID_UNIT_REGISTER_INTERFACE_SIGN_S
#define RAID_UNIT_REGISTER_INTERFACE_MASK	VMProtectDecryptStringA(RAID_UNIT_REGISTER_INTERFACE_MASK_S)

#define DEVICE_RAID_PORT					VMProtectDecryptStringW(DEVICE_RAID_PORT_S)

#define DISK_SYS							VMProtectDecryptStringA(DISK_SYS_S)
#define DISK_DISABLE_PREDICTION_SIGN		DISK_DISABLE_PREDICTION_SIGN_S
#define DISK_DISABLE_PREDICTION_MASK		VMProtectDecryptStringA(DISK_DISABLE_PREDICTION_MASK_S)
#define DRIVER_DISK							VMProtectDecryptStringW(DRIVER_DISK_S)


#define NTOSKRNL							VMProtectDecryptStringA(NTOSKRNL_S)

#define SMBIOS_SIGN1						SMBIOS_SIGN1_S
#define SMBIOS_MASK1						VMProtectDecryptStringA(SMBIOS_MASK1_S)

#define SMBIOS_SIGN2						SMBIOS_SIGN2_S
#define SMBIOS_MASK2						VMProtectDecryptStringA(SMBIOS_MASK2_S)
#endif