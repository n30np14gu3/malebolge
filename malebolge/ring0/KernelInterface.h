#pragma once
#include "driver_io.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

struct PModule
{
	DWORD32 dwSize;
	DWORD32 dwBase;
};

class KernelInterface
{
public:
	KernelInterface(LPCSTR hRegistryPath);

	template <typename T>
	void Read(DWORD64 address, T* result)
	{
		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_READ_REQUEST req{ address, (ULONG)(result), sizeof(T), -1 };
		DeviceIoControl(m_hDriver, IO_READ_PROCESS_MEMORY, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	}

	template <typename T>
	void Write(DWORD64 address, T val);


	bool Attach(LPCSTR szProcessName);
	PModule GetModule(LPCSTR szModuleName);

	~KernelInterface();
private:
	HANDLE m_hDriver;
	DWORD32 m_dwProcessId;
	
};

