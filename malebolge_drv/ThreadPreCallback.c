#include <ntifs.h>
#include "globals.h"
#include "callback.h"

void DisplayHandleStrip(ACCESS_MASK mask)
{
#if DEBUG
	PRINTF("Stripping handle [0x%X]", mask);
#endif
}

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	
	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;


	if (PsGetCurrentProcessId() == PROTECTED_PROCESS)
		return OB_PREOP_SUCCESS;

	if(PsGetThreadProcessId(OperationInformation->Object) == PROTECTED_PROCESS)
	{

		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
			DisplayHandleStrip(OperationInformation->Parameters->CreateHandleInformation.DesiredAccess);
		}
		else
		{
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
			DisplayHandleStrip(OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess);
		}
	}
	
	return OB_PREOP_SUCCESS;
}