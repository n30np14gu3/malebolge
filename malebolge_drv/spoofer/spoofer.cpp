#include <ntifs.h>
#include "shared.h"

#include "disks.h"
#include "smbios.h"

extern "C" void spoof()
{
	Disks::DisableSmart();
	Disks::ChangeDiskSerials();
	Smbios::ChangeSmbiosSerials();
}

