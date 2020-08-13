#pragma once

//Functions (Calls)
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

//Driver unloading
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);