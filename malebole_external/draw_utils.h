#pragma once
namespace draw_utils
{
	void init_utils(HWND hWindow, RECT winRect);

	bool w2s(D3DXVECTOR3 world, D3DXVECTOR2& screen, const float flMatrix[4][4]);

	void line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color);
	void box(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, D3DCOLOR color);
	void healthBox(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, DWORD32 health, D3DCOLOR color, D3DCOLOR healthColor);
	void string(const D3DXVECTOR2& pos, const std::string& text, D3DCOLOR color);
	void crosshair(DWORD size, D3DXVECTOR2 position, D3DCOLOR color);
	void render(void* ptr);
	void hackProc(void* ptr);

	extern LPDIRECT3DDEVICE9 m_dxDevice;
	extern LPD3DXFONT m_dxFont;
	extern D3DPRESENT_PARAMETERS m_d3Params;
	extern HWND m_hWindow;
	extern RECT m_rDesktop;
	extern INT m_iWidth;
	extern INT m_iHeight;
};
