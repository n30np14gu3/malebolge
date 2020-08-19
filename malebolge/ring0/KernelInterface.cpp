#include <Windows.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "KernelInterface.h"
#include "../SDK/globals.h"

KernelInterface::KernelInterface()
{
	m_hDriver = CreateFile(DRIVER_NAME, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	m_dwErrorCode = 0;
	if(m_hDriver == INVALID_HANDLE_VALUE)
	{
		m_dwErrorCode = GetLastError();
	}

	NoErrors = true;
}

bool KernelInterface::Attach()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	do
		if (!strcmp(_bstr_t(entry.szExeFile), GAME_NAME)) {
			m_dwProcessId = entry.th32ProcessID;
			CloseHandle(hSnapshot);
		}
	while (Process32Next(hSnapshot, &entry));
	if (m_dwProcessId == 0)
		return false;

	KERNEL_INIT_DATA_REQUEST req{ m_dwProcessId, GetCurrentProcessId(), -1 };
	
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return false;
	
	BOOL result = DeviceIoControl(m_hDriver, IO_INIT_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if(!result)
	{
		m_dwErrorCode = GetLastError();
		NoErrors = false;
		return false;
	}

	if(!NT_SUCCESS(req.Result))
	{
		m_dwErrorCode = req.Result;
		NoErrors = false;
		return false;
	}

	return true;
}

bool KernelInterface::GetModules()
{
	return false;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
}

DWORD KernelInterface::GetErrorCode() const
{
	return m_dwErrorCode;
}

