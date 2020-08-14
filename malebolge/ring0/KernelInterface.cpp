#include <Windows.h>
#include "KernelInterface.h"


KernelInterface::KernelInterface(LPCSTR szRegistryPath)
{
	m_dwProcessId = 13500;
	m_hDriver = CreateFile(szRegistryPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if(m_hDriver == INVALID_HANDLE_VALUE)
		return;

	PKERNEL_INIT_DATA_REQUEST init = new KERNEL_INIT_DATA_REQUEST();
	init->Result = -1;
	init->CheatId = GetCurrentProcessId();
	init->CsgoId = m_dwProcessId;
	DeviceIoControl(m_hDriver, IO_INIT_CHEAT_DATA, init, sizeof(KERNEL_INIT_DATA_REQUEST), init, sizeof(KERNEL_INIT_DATA_REQUEST), nullptr, nullptr);
}

bool KernelInterface::Attach(LPCSTR szProcessName)
{
	return false;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
}

PModule KernelInterface::GetModule(LPCSTR szModuleName)
{
	return { };
}

