#include <Windows.h>
#include <SubAuth.h>
#include <TlHelp32.h>
#include <comdef.h>

#include <string>

#include "../SDK/globals.h"
#include "../SDK/VmpSdk.h"
#include "KernelInterface.h"
#include "DriverLoader.h"

typedef BOOLEAN (WINAPI* pRtlDosPathNameToNtPathName_U)(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR* FilePart, PVOID RelativeName);
typedef void(WINAPI* pRtlFreeUnicodeString)(PUNICODE_STRING UnicodeString);


KernelInterface::KernelInterface()
{
	VM_START("KernelInterface::KernelInterface");
	m_hDriver = LI_FN(CreateFileA)(ENCRYPT_STR(DRIVER_NAME),
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
		//const char* fName = ENCRYPT_STR("C:\\Windows\\System32\\drivers\\check.sys");
		//FILE* f = nullptr;
		//fopen_s(&f, fName, "wb");
		//fwrite(drv, 1, sizeof(drv), f);
		//fclose(f);
		//srand(GetTickCount());
		//
		//DriverLoader loader(fName, rnd_str());
		//if (!loader.LoadDriver())
		//{
		//	m_dwErrorCode = 0x100;
		//	return;
		//}

		//m_hDriver = LI_FN(CreateFileA)(ENCRYPT_STR(DRIVER_NAME),
		//	GENERIC_READ | GENERIC_WRITE,
		//	FILE_SHARE_READ | FILE_SHARE_WRITE,
		//	nullptr,
		//	OPEN_EXISTING,
		//	0,
		//	nullptr);
		//
		//if (m_hDriver == INVALID_HANDLE_VALUE)
		//{
		//	m_dwErrorCode = LI_FN(GetLastError)();
		//	return;
		//}
		m_dwErrorCode = LI_FN(GetLastError)();
		return;

	}
	NoErrors = true;
	VM_END;
}


bool KernelInterface::Attach(bool update)
{
	HANDLE hSnapshot = LI_FN(CreateToolhelp32Snapshot).cached()(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(entry);
	do
		if (!strcmp(_bstr_t(entry.szExeFile), ENCRYPT_STR(GAME_NAME))) {
			m_dwProcessId = entry.th32ProcessID;
			m_hProcess = LI_FN(OpenProcess).cached()(SYNCHRONIZE, false, m_dwProcessId);
			LI_FN(CloseHandle).cached()(hSnapshot);
		}
	while (LI_FN(Process32Next).cached()(hSnapshot, &entry));
	if (m_dwProcessId == 0)
		return false;

	KERNEL_INIT_DATA_REQUEST req{ m_dwProcessId, LI_FN(GetCurrentProcessId).cached()(), -1 };

	if (m_hDriver == INVALID_HANDLE_VALUE)
		return false;

	const BOOL result = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_INIT_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	if (!result)
	{
		m_dwErrorCode = LI_FN(GetLastError).cached()();
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
	BOOL result = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_GET_ALL_MODULES, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
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
	LI_FN(DeviceIoControl).cached()(m_hDriver, IO_DRIVER_ALIVE, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	return !req.status;
}

KernelInterface::~KernelInterface()
{
	LI_FN(CloseHandle).cached()(m_hDriver);
	LI_FN(CloseHandle).cached()(m_hProcess);
	delete Modules;
}

DWORD KernelInterface::GetErrorCode() const
{
	return m_dwErrorCode;
}

void KernelInterface::GetDriverStatus(bool& bbIsOn, bool& driverIsInited)
{
	if (!m_hDriver)
		return;
	DRIVER_STATUS_REQUEST req{ -1 };
	if(!LI_FN(DeviceIoControl).cached()(m_hDriver, IO_GET_DRIVER_STATUS, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr))
		return;

	bbIsOn = req.bbInited;
	driverIsInited = req.bbInited;
}


void KernelInterface::WaitForProcessClose()
{
	LI_FN(WaitForSingleObjectEx).cached()(m_hProcess, INFINITE, TRUE);
}
