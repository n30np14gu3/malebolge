#pragma once

class DriverLoader
{
public:
	DriverLoader(const std::string& driverPath, const std::string& driverName);
	bool LoadDriver() const;
	bool UnloadDriver() const;
private:

	std::string m_sDriverName;
	std::string m_sDriverPath;
};