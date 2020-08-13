#include <ntifs.h>
#include "globals.h"
#include "callback.h"

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	if (PROTECTED_PROCESS == 0)
		return OB_PREOP_SUCCESS;

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;


	if (PsGetProcessId((PEPROCESS)OperationInformation->Object) == PROTECTED_PROCESS)
	{

		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE);
		}
		else
		{
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE);
		}
		return OB_PREOP_SUCCESS;
	}
	
	return OB_PREOP_SUCCESS;
}