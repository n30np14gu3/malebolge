#include <ntifs.h>
#include "globals.h"
#include "callback.h"

PVOID OB_HANDLE = NULL;

ULONG Lsass;
ULONG Csrss1;
ULONG Csrss2;


void DisplayError(NTSTATUS result)
{
#if DEBUG
	if (!NT_SUCCESS(result))
	{
		switch (result)
		{
		case STATUS_FLT_INSTANCE_ALTITUDE_COLLISION:
			PRINTF("Altitude is already exist!");
			break;

		case STATUS_INVALID_PARAMETER:
			PRINTF("Parameter is error!");
			break;

		case STATUS_ACCESS_DENIED:
			PRINTF("The callback routines do not reside in a signed kernel binary image!");
			break;

		case STATUS_INSUFFICIENT_RESOURCES:
			PRINTF("An attempt to allocate memory failed!");
			break;
		default:
			DbgPrintEx(0, 0, "ObRegisterCallbacks error! [0x%X]", result);
			break;
		}
	}
#endif
}

void EnableCallback()
{
#ifndef DEBUG
	VMProtectBeginUltra("#EnableCallback");
#endif
	NTSTATUS result;

	OB_OPERATION_REGISTRATION OBOperationRegistration[2];
	OB_CALLBACK_REGISTRATION OBOCallbackRegistration;
	UNICODE_STRING usAltitude;
	memset(&OBOperationRegistration, 0, sizeof(OB_OPERATION_REGISTRATION));
	memset(&OBOCallbackRegistration, 0, sizeof(OB_CALLBACK_REGISTRATION));
	RtlInitUnicodeString(&usAltitude, L"1000");

	if ((USHORT)ObGetFilterVersion() == OB_FLT_REGISTRATION_VERSION)
	{
		OBOperationRegistration[1].ObjectType = PsProcessType;
		OBOperationRegistration[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		OBOperationRegistration[1].PreOperation = ProcessPreCallback;
		OBOperationRegistration[1].PostOperation = NULL;


		OBOperationRegistration[0].ObjectType = PsThreadType;
		OBOperationRegistration[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		OBOperationRegistration[0].PreOperation = ThreadPreCallback;
		OBOperationRegistration[0].PostOperation = NULL;

		OBOCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
		OBOCallbackRegistration.OperationRegistrationCount = 2;
		OBOCallbackRegistration.RegistrationContext = NULL;
		OBOCallbackRegistration.OperationRegistration = (OB_OPERATION_REGISTRATION*)&OBOperationRegistration;

		result = ObRegisterCallbacks(&OBOCallbackRegistration, &OB_HANDLE);
		if (!NT_SUCCESS(result))
		{
			DisplayError(result);
		}
	}

#ifndef DEBUG
	VMProtectEnd();
#endif
}

void DisableCallback()
{
	if(OB_HANDLE == NULL)
		return;

	ObUnRegisterCallbacks(OB_HANDLE);
	OB_HANDLE = NULL;
}