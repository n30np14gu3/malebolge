#include <Windows.h>
#include <SubAuth.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "KernelInterface.h"
#include "../SDK/globals.h"
#include "../themida_sdk/Themida.h"

typedef BOOLEAN (WINAPI* pRtlDosPathNameToNtPathName_U)(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR* FilePart, PVOID RelativeName);
typedef void(WINAPI* pRtlFreeUnicodeString)(PUNICODE_STRING UnicodeString);

KernelInterface::KernelInterface()
{
	PROTECT_VM_START_HIGH;
	m_hDriver = CreateFile(DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr);

	m_dwErrorCode = 0;
	m_dwProcessId = 0;
	m_hProcess = INVALID_HANDLE_VALUE;
	Modules = new CSGoModules();
	if (m_hDriver == INVALID_HANDLE_VALUE)
	{
		m_dwErrorCode = GetLastError();
		return;
	}
	NoErrors = true;
	PROTECT_VM_END_HIGH;
}

bool KernelInterface::Inject(const wchar_t* szDll) const
{
	//Nope in external soft
	return true;
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

	const BOOL result = DeviceIoControl(m_hDriver, IO_INIT_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
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

	Modules->bServer = req.bServer;
	Modules->bClient = req.bClient;
	Modules->bEngine = req.bEngine;
	return true;
}

bool KernelInterface::IsAlive() const
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;

	DRIVER_ALIVE_REQUEST req{ -1 };
	DeviceIoControl(m_hDriver, IO_DRIVER_ALIVE, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	return !req.status;
}

KernelInterface::~KernelInterface()
{
	CloseHandle(m_hDriver);
	CloseHandle(m_hProcess);
	delete Modules;
}

DWORD KernelInterface::GetErrorCode() const
{
	return m_dwErrorCode;
}

void KernelInterface::EnableBB()
{
	if(!m_hDriver)
		return;

	BOOL result = FALSE;
	DeviceIoControl(m_hDriver, IO_ENABLE_BB, &result, sizeof(result), &result, sizeof(result), nullptr, nullptr);
}


void KernelInterface::GetDriverStatus(bool& bbIsOn, bool& driverIsInited)
{
	if (!m_hDriver)
		return;
	DRIVER_STATUS_REQUEST req{ -1 };
	if(!DeviceIoControl(m_hDriver, IO_GET_DRIVER_STATUS, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr))
		return;

	bbIsOn = req.bbInited;
	driverIsInited = req.bbInited;
}


void KernelInterface::WaitForProcessClose()
{
	WaitForSingleObjectEx(m_hProcess, INFINITE, TRUE);
}