#pragma once
#include "driver_io.h"

struct CSGoModules
{
	DWORD32 bClient;
	DWORD32 bEngine;
	DWORD32 bServer;
};

class KernelInterface
{
public:
	BOOLEAN NoErrors;
	CSGoModules* Modules;
	KernelInterface();
	
	bool Inject(const wchar_t* szDll) const;
	bool Attach(bool update = false);
	bool GetModules();
	bool IsAlive() const;
	void WaitForProcessClose();
	DWORD GetErrorCode() const;
	~KernelInterface();
private:
	HANDLE m_hDriver;
	DWORD32 m_dwProcessId;
	DWORD m_dwErrorCode;
	HANDLE m_hProcess;
	
};

