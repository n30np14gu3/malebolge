/*
 * Mutante
 * Made by Samuel Tulach
 * https://github.com/SamuelTulach/mutante
 */

#include <ntifs.h>
#include "log.h"
#include "shared.h"

#include "disks.h"
#include "smbios.h"

extern "C" void spoof()
{
	Disks::DisableSmart();
	Disks::ChangeDiskSerials();
	Smbios::ChangeSmbiosSerials();
}

