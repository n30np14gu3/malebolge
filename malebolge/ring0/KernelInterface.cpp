#include <Windows.h>
#include "KernelInterface.h"
#include "driver_io.h"

KernelInterface::KernelInterface(LPCSTR szRegistryPath)
{
	m_dwProcessId = 9908;
	m_hDriver = CreateFile(szRegistryPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if(m_hDriver == INVALID_HANDLE_VALUE)
		return;

	KERNEL_INIT_DATA_REQUEST init{ m_dwProcessId, GetCurrentProcessId() };
	DWORD b = 0;
	DeviceIoControl(m_hDriver, IO_INIT_CHEAT_DATA, &init, sizeof(init), &init, sizeof(init), &b, 0);
}

template <typename T>
void KernelInterface::Write(DWORD32 address, T val)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return;
}

template <typename T>
T KernelInterface::Read(DWORD32 address)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return static_cast<T>(false);

	return false;
}

bool KernelInterface::Attach(LPCSTR szProcessName)
{
	return false;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
}
