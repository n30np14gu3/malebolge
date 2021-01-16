#include <Windows.h>

#include <d3dx9.h>
#include <d3d9.h>

#include <string>

#include "lazy_importer.hpp"
#include "draw_utils.h"
#include "global_defs.h"

unsigned char ARIAL_S[] = { 0x41, 0x73, 0x6B, 0x62, 0x68, 0x05 };

namespace draw_utils
{
	INT m_iWidth;
	INT m_iHeight;

	IDirect3DDevice9* m_dxDevice;
	LPD3DXFONT m_dxFont;
	RECT m_rDesktop;
}

void draw_utils::init_draw_utils(IDirect3DDevice9* device, RECT winRect)
{
	m_dxFont = nullptr;
	
	m_dxDevice = device;
	m_rDesktop = winRect;

	m_iHeight = m_rDesktop.bottom - m_rDesktop.top;
	m_iWidth = m_rDesktop.right - m_rDesktop.left;

	decrypt(ARIAL_S, sizeof(ARIAL_S));
	LI_FN(D3DXCreateFontA)(
		m_dxDevice,
		16,
		0,
		FW_NORMAL,
		1,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		reinterpret_cast<const char*>(ARIAL_S),
		&m_dxFont);
	memset(ARIAL_S, 0, sizeof(ARIAL_S));
}



bool draw_utils::w2s(D3DXVECTOR3 world, D3DXVECTOR2& screen,float* flMatrix)
{
	float w;

	screen.x = flMatrix[0] * world.x + flMatrix[1] * world.y + flMatrix[2] * world.z + flMatrix[3];
	screen.y = flMatrix[4] * world.x + flMatrix[5] * world.y + flMatrix[6] * world.z + flMatrix[7];
	w = flMatrix[8] * world.x + flMatrix[9] * world.y + flMatrix[10] * world.z + flMatrix[11];

	if (w < 0.01f)
		return false;

	const float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	int width = static_cast<int>(m_rDesktop.right - m_rDesktop.left);
	int height = static_cast<int>(m_rDesktop.bottom - m_rDesktop.top);

	float x = static_cast<float>(width) / 2;
	float y = static_cast<float>(height) / 2;

	x += static_cast<float>(screen.x * x);
	y -= static_cast<float>(screen.y * y);

	screen.x = x + m_rDesktop.left;
	screen.y = y + m_rDesktop.top;

	return true;
}

void draw_utils::line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color)
{
	if (m_dxDevice == nullptr)
		return;

	ID3DXLine* line;
	LI_FN(D3DXCreateLine)(m_dxDevice, &line);
	D3DXVECTOR2 linePos[] = { from , to };
	line->SetWidth(width);
	line->SetAntialias(true);
	line->Begin();
	line->Draw(linePos, 2, color);
	line->End();
	line->Release();
}

void draw_utils::box(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, D3DCOLOR color)
{
	if (m_dxDevice == nullptr)
		return;

	ID3DXLine* p_Line;
	LI_FN(D3DXCreateLine)(m_dxDevice, &p_Line);
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	p_Line->SetWidth(px);
	p_Line->Draw(points, 5, color);
	p_Line->End();
	p_Line->Release();
}

void draw_utils::healthBox(
	FLOAT x,
	FLOAT y,
	FLOAT width,
	FLOAT height,
	FLOAT px,
	DWORD32 health,
	D3DCOLOR color,
	D3DCOLOR healthColor)
{
	if (m_dxDevice == nullptr)
		return;

	box(x, y, width, height, px, color);

	ID3DXLine* p_LineHealth;
	LI_FN(D3DXCreateLine)(m_dxDevice, &p_LineHealth);
	const FLOAT healthHeight = height - (health * height / 100);
	D3DXVECTOR2 linePos[] = { D3DXVECTOR2(x - 2.f, y + height), D3DXVECTOR2(x - 2.f, y + healthHeight) };
	p_LineHealth->SetWidth(1.f);
	p_LineHealth->SetAntialias(true);
	p_LineHealth->Draw(linePos, 2, healthColor);
	p_LineHealth->Release();
}

void draw_utils::string(const D3DXVECTOR2& pos, const char* text, D3DCOLOR color)
{
	RECT fontPos = {
		static_cast<LONG>(pos.x),
		static_cast<LONG>(pos.y),
		static_cast<LONG>(pos.x) + 120,
		static_cast<LONG>(pos.y) + 16
	};
	m_dxFont->DrawTextA(nullptr, text, -1, &fontPos, DT_NOCLIP, color);
}

void draw_utils::crosshair(DWORD size, D3DXVECTOR2 position, D3DCOLOR color)
{
	D3DXVECTOR2 l, r, t, b;
	l = r = t = b = position;
	l.x -= size;
	r.x += size;
	b.y += size;
	t.y -= size;
	line(l, r, 2, color);
	line(t, b, 2, color);
}
