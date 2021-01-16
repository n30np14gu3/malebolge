#pragma once

namespace draw_utils
{
	void init_draw_utils(IDirect3DDevice9* device, RECT winRect);
	bool w2s(D3DXVECTOR3 world, D3DXVECTOR2& screen, float* flMatrix);
	void line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color);
	void box(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, D3DCOLOR color);
	void healthBox(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, DWORD32 health, D3DCOLOR color, D3DCOLOR healthColor);
	void string(const D3DXVECTOR2& pos, const char* text, D3DCOLOR color);
	void crosshair(DWORD size, D3DXVECTOR2 position, D3DCOLOR color);

	extern INT m_iWidth;
	extern INT m_iHeight;
	
	extern IDirect3DDevice9* m_dxDevice;
	extern LPD3DXFONT m_dxFont;
	extern RECT m_rDesktop;
}
