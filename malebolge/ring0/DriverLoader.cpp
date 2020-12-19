#include "../SDK/globals.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "DriverLoader.h"

DriverLoader::DriverLoader(const std::string& driverPath, const std::string& driverName)
{	
	m_sDriverName = driverName;
	m_sDriverPath = "";

	if (std::filesystem::exists(driverPath))
		m_sDriverPath = driverPath;
}

bool DriverLoader::LoadDriver() const
{
	const SC_HANDLE scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
		return false;

	
	SC_HANDLE sh = CreateService(
		scm,
		m_sDriverName.c_str(),
		m_sDriverName.c_str(),
		SERVICE_START | DELETE,
		SERVICE_KERNEL_DRIVER,
		SERVICE_BOOT_START,
		SERVICE_ERROR_IGNORE,
		m_sDriverPath.c_str(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	if(!sh)
	{
		sh = OpenService(scm, m_sDriverName.c_str(), SERVICE_START);
		if(!sh)
		{
			CloseHandle(scm);
			return false;
		}
	}

	const bool result = StartService(sh, 0, nullptr);
	CloseHandle(scm);
	CloseHandle(sh);
	return result;
}

bool DriverLoader::UnloadDriver() const
{
	const SC_HANDLE sc_manager_handle = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);

	if (!sc_manager_handle)
		return false;

	const SC_HANDLE service_handle = OpenService(sc_manager_handle, m_sDriverName.c_str(), DELETE);

	if (!service_handle)
	{
		CloseServiceHandle(sc_manager_handle);
		return false;
	}

	SERVICE_STATUS status = { 0 };
	const bool result = ControlService(service_handle, SERVICE_CONTROL_STOP, &status) && DeleteService(service_handle);

	CloseServiceHandle(service_handle);
	CloseServiceHandle(sc_manager_handle);
	return result;
}
