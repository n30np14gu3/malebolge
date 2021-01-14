#pragma once
class draw_utils
{
public:
	draw_utils(HWND hWindow, RECT winRect);

	bool w2s(D3DXVECTOR3 world, D3DXVECTOR2& screen, const float flMatrix[4][4]) const;

	void line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color) const;
	void box(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, D3DCOLOR color) const;
	void healthBox(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, DWORD32 health, D3DCOLOR color, D3DCOLOR healthColor) const;
	void string(const D3DXVECTOR2& pos, const std::string& text, D3DCOLOR color) const;
	void crosshair(DWORD size, D3DXVECTOR2 position, D3DCOLOR color) const;
	void render(void* ptr);
private:
	LPDIRECT3DDEVICE9 m_dxDevice;
	LPD3DXFONT m_dxFont;
	HWND m_hWindow;
	RECT m_rDesktop;
	INT m_iWidth;
	INT m_iHeight;
	void hackProc(void* ptr);
};