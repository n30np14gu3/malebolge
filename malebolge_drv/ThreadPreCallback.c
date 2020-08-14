#include <ntifs.h>
#include "globals.h"
#include "callback.h"

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
#ifndef DEBUG
	VMProtectBeginMutation("#ThreadPreCallback");
#endif
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
		}
		else
		{
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
		}
	}

#ifndef DEBUG
	VMProtectEnd();
#endif
	return OB_PREOP_SUCCESS;
}