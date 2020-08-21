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
	m_dwProcessId = 0;
	m_hProcess = INVALID_HANDLE_VALUE;
	Modules = CSGoModules{ 0, 0, 0 };
	if (m_hDriver == INVALID_HANDLE_VALUE)
	{
		m_dwErrorCode = GetLastError();
	}
	NoErrors = true;
}

bool KernelInterface::Attach(bool update)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	do
		if (!strcmp(_bstr_t(entry.szExeFile), GAME_NAME)) {
			m_dwProcessId = entry.th32ProcessID;
			m_hProcess = OpenProcess(SYNCHRONIZE, false, m_dwProcessId);
			CloseHandle(hSnapshot);
		}
	while (Process32Next(hSnapshot, &entry));
	if (m_dwProcessId == 0)
		return false;

	KERNEL_INIT_DATA_REQUEST req{ m_dwProcessId, GetCurrentProcessId(), -1 };

	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;

	BOOL result = DeviceIoControl(m_hDriver, !update ? IO_INIT_CHEAT_DATA : IO_UPDATE_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if (!result)
	{
		m_dwErrorCode = GetLastError();
		NoErrors = false;
		return false;
	}

	if (!NT_SUCCESS(req.Result))
	{
		m_dwErrorCode = req.Result;
		NoErrors = false;
		return false;
	}

	return true;
}

bool KernelInterface::GetModules()
{
	NoErrors = false;

	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;


	KERNEL_GET_CSGO_MODULES req{ 0, 0, 0, -1 };
	BOOL result = DeviceIoControl(m_hDriver, IO_GET_ALL_MODULES, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if (!result)
		return false;

	if (!NT_SUCCESS(req.result))
		return false;

	Modules.bServer = req.bServer;
	Modules.bClient = req.bClient;
	Modules.bEngine = req.bEngine;
	return true;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
	CloseHandle(m_hProcess);
}

DWORD KernelInterface::GetErrorCode() const
{
	return m_dwErrorCode;
}

void KernelInterface::WaitForProcessClose()
{
	WaitForSingleObjectEx(m_hProcess, INFINITE, TRUE);
	while (!Attach(true)) {}
	while (!GetModules()) {}
}


