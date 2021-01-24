#pragma once

class Menu
{
public:
	Menu(const char* title, void* ptr);
	virtual bool SetupWindow();
	virtual void Render();
	virtual ~Menu();
private:
	virtual bool CreateD3D();
	HWND m_hWindow;
	WNDCLASSEX wc;
	
	LPDIRECT3D9 m_d3D;
	LPDIRECT3DDEVICE9 m_dxDevice;
	D3DPRESENT_PARAMETERS m_d3dParams;
	
	INT m_iWidth;
	INT m_iHeight;
	
	const char* m_szTitle;

	void* m_kDriver;
};
