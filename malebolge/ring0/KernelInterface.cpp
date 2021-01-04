#include <Windows.h>
#include <SubAuth.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "KernelInterface.h"
#include "../SDK/globals.h"
#include "../themida_sdk/ThemidaSDK.h"


typedef BOOLEAN (WINAPI* pRtlDosPathNameToNtPathName_U)(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR* FilePart, PVOID RelativeName);
typedef void(WINAPI* pRtlFreeUnicodeString)(PUNICODE_STRING UnicodeString);

KernelInterface::KernelInterface()
{
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
	}
	NoErrors = true;
}

bool KernelInterface::Inject(const wchar_t* szDll) const
{
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return false;
	
	DWORD bytes = 0;
	INJECT_DLL data = { IT_MMap };
	UNICODE_STRING ustr = { 0 };

	StrEncryptStart();
	HMODULE hNtdll = GetModuleHandle(S_ntdll);
	if(hNtdll == nullptr)
		return false;

	pRtlDosPathNameToNtPathName_U pFunc = reinterpret_cast<pRtlDosPathNameToNtPathName_U>(GetProcAddress(hNtdll, S_RtlDosPathNameToNtPathName_U));
	pRtlFreeUnicodeString pFreeFunc = reinterpret_cast<pRtlFreeUnicodeString>(GetProcAddress(hNtdll, S_RtlFreeUnicodeString));
	StrEncryptEnd();
	
	if(pFunc == nullptr || pFreeFunc == nullptr)
		return false;

	pFunc(szDll, &ustr, nullptr, nullptr);

	if(ustr.Buffer == nullptr)
		return false;
	
	wcscpy_s(data.FullDllPath, ustr.Buffer);

	pFreeFunc(&ustr);
	
	data.pid = m_dwProcessId;
	data.initRVA = 0;
	data.wait = false;
	data.unlink = true;
	data.erasePE = false;
	data.flags = KNoFlags;
	data.imageBase = 0;
	data.imageSize = 0;
	data.asImage = false;

	if(!DeviceIoControl(m_hDriver, IO_INJECT_DLL, &data, sizeof(data), nullptr, 0, &bytes, nullptr))
		return false;

	memset(&data, 0, sizeof(data));

	
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

void KernelInterface::WaitForProcessClose()
{
	WaitForSingleObjectEx(m_hProcess, INFINITE, TRUE);
}
