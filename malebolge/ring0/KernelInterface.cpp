#include <Windows.h>
#include <SubAuth.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "KernelInterface.h"
#include "../SDK/globals.h"

typedef BOOLEAN (_stdcall *pRtlDosPathNameToNtPathName_U)(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR* FilePart, PVOID RelativeName);

KernelInterface::KernelInterface()
{
	m_hDriver = CreateFile(DRIVER_NAME, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

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

void KernelInterface::Inject(const wchar_t* szDll)
{
	if(m_hDriver == INVALID_HANDLE_VALUE)
		return;
	
	DWORD bytes = 0;
	INJECT_DLL data = { IT_MMap };
	UNICODE_STRING ustr = { 0 };

	HMODULE hNtdll = GetModuleHandle("ntdll.dll");
	if(hNtdll == nullptr)
		return;

	pRtlDosPathNameToNtPathName_U pFunc = reinterpret_cast<pRtlDosPathNameToNtPathName_U>(GetProcAddress(hNtdll, "RtlDosPathNameToNtPathName_U"));
	if(pFunc == nullptr)
		return;

	pFunc(szDll, &ustr, nullptr, nullptr);
	wcscpy_s(data.FullDllPath, ustr.Buffer);

	data.pid = m_dwProcessId;
	data.initRVA = 0;
	data.wait = false;
	data.unlink = false;
	data.erasePE = false;
	data.flags = KNoFlags;
	data.imageBase = 0;
	data.imageSize = 0;
	data.asImage = false;

	if(!DeviceIoControl(m_hDriver, IO_INJECT_DLL, &data, sizeof(data), nullptr, 0, &bytes, nullptr))
		return;

	memset(&data, 0, sizeof(data));
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

	const BOOL result = DeviceIoControl(m_hDriver, !update ? IO_INIT_CHEAT_DATA : IO_UPDATE_CHEAT_DATA, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
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

void KernelInterface::WaitForProcessClose()
{
	WaitForSingleObjectEx(m_hProcess, INFINITE, TRUE);
	while (!Attach(true)) {}
	while (!GetModules()) {}
}
