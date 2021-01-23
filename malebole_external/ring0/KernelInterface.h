#pragma once
#include "../SDK/lazy_importer.hpp"
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
	virtual bool Attach(bool update = false);
	virtual bool GetModules();
	virtual bool IsAlive() const;
	virtual void WaitForProcessClose();
	virtual DWORD GetErrorCode() const;
	virtual void GetDriverStatus(bool& bbIsOn, bool& driverIsInited);
	virtual ~KernelInterface();

	template <typename T>
	_inline void Read(DWORD64 address, T* result)
	{
		NoErrors = false;

		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_READ_REQUEST req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Response = (DWORD64)result;
		req.Result = -1;

		BOOL rsp = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_READ_PROCESS_MEMORY, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = LI_FN(GetLastError).cached()();
		}
		else
		{
			if (!NT_SUCCESS(req.Result))
			{
				m_dwErrorCode = req.Result;
			}
		}

		NoErrors = true;
	}

	template <typename T>
	_inline void Read32(DWORD32 address, T* result)
	{
		NoErrors = false;

		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_READ_REQUEST32 req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Response = (DWORD64)result;
		req.Result = -1;

		BOOL rsp = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_READ_PROCESS_MEMORY_32, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = LI_FN(GetLastError).cached()();
		}
		else
		{
			if (!NT_SUCCESS(req.Result))
			{
				m_dwErrorCode = req.Result;
			}
		}

		NoErrors = true;

	}

	template <typename T>
	_inline void Write(DWORD64 address, T* val)
	{
		NoErrors = false;

		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_WRITE_REQUEST req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Value = (DWORD64)val;
		req.Result = -1;

		BOOL rsp = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_WRITE_PROCESS_MEMORY, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = LI_FN(GetLastError).cached()();
		}
		else
		{
			if (!NT_SUCCESS(req.Result))
			{
				m_dwErrorCode = req.Result;
			}
		}

		NoErrors = true;
	}

	template <typename T>
	_inline void Write32(DWORD32 address, T* val)
	{
		NoErrors = false;

		if (m_hDriver == INVALID_HANDLE_VALUE)
			return;

		KERNEL_WRITE_REQUEST32 req;
		req.Size = sizeof(T);
		req.Address = address;
		req.Value = (DWORD64)val;
		req.Result = -1;

		BOOL rsp = LI_FN(DeviceIoControl).cached()(m_hDriver, IO_WRITE_PROCESS_MEMORY_32, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
		if (!rsp)
		{
			m_dwErrorCode = LI_FN(GetLastError).cached()();
		}
		else
		{
			if (!NT_SUCCESS(req.Result))
			{
				m_dwErrorCode = req.Result;
			}
		}

		NoErrors = true;
	}
	
private:
	HANDLE m_hDriver;
	DWORD32 m_dwProcessId;
	DWORD m_dwErrorCode;
	HANDLE m_hProcess;
	
};
