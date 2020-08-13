#pragma once
struct PModule
{
	DWORD32 dwSize;
	DWORD32 dwBase;
};

class KernelInterface
{
public:
	KernelInterface(LPCSTR hRegistryPath);

	template <typename T>
	T Read(DWORD32 address);

	template <typename T>
	void Write(DWORD32 address, T val);


	bool Attach(LPCSTR szProcessName);
	PModule GetModule(LPCSTR szModuleName);

	~KernelInterface();
private:
	HANDLE m_hDriver;
	DWORD32 m_dwProcessId;
	
};

