#include "globals.h"
#include "CreateProcessCallback.h"

void CreateProcessCallback(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	if(!Create)
	{
		//Game closed
		if (ProcessId == PROTECTED_PROCESS || ProcessId == GAME_PROCESS)
			DRIVER_INITED = FALSE;
	}
	
}