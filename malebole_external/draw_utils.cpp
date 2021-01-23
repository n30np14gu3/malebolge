#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "draw_utils.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/XorStr.hpp"


typedef HRESULT(WINAPI* D3DXCreateLineFn)(LPDIRECT3DDEVICE9   pDevice, LPD3DXLINE* ppLine);
D3DXCreateLineFn createLine;
D3DPRESENT_PARAMETERS d3dpp;

draw_utils::draw_utils(HWND hWindow, RECT winRect)
{
	LI_FN(LoadLibraryA)(xorstr("d3d9.dll").crypt_get());
	LI_FN(LoadLibraryA)(xorstr("d3dx9_43.dll").crypt_get());
	createLine = LI_FN(D3DXCreateLine).get();

	m_dxDevice = nullptr;
	m_dxFont = nullptr;

	m_hWindow = hWindow;
	m_rDesktop = winRect;

	m_iHeight = m_rDesktop.bottom - m_rDesktop.top;
	m_iWidth = m_rDesktop.right - m_rDesktop.left;

	LPDIRECT3D9 d3d = LI_FN(Direct3DCreate9)(D3D_SDK_VERSION);
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_hWindow;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferWidth = m_iWidth;
	d3dpp.BackBufferHeight = m_iHeight;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_dxDevice);

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
		xorstr("Arial").crypt_get(),
		&m_dxFont);
}

bool draw_utils::w2s(const D3DXVECTOR3 world, D3DXVECTOR2& screen, const float flMatrix[4][4]) const
{
	float w;

	screen.x = flMatrix[0][0] * world.x + flMatrix[0][1] * world.y + flMatrix[0][2] * world.z + flMatrix[0][3];
	screen.y = flMatrix[1][0] * world.x + flMatrix[1][1] * world.y + flMatrix[1][2] * world.z + flMatrix[1][3];
	w = flMatrix[3][0] * world.x + flMatrix[3][1] * world.y + flMatrix[3][2] * world.z + flMatrix[3][3];

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

void draw_utils::line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color) const
{
	if (m_dxDevice == nullptr)
		return;

	ID3DXLine* line;
	createLine(m_dxDevice, &line);
	D3DXVECTOR2 linePos[] = { from , to };
	line->SetWidth(width);
	line->SetAntialias(true);
	line->Begin();
	line->Draw(linePos, 2, color);
	line->End();
	line->Release();
}

void draw_utils::box(FLOAT x, FLOAT y, FLOAT width, FLOAT height, FLOAT px, D3DCOLOR color) const
{
	if (m_dxDevice == nullptr)
		return;

	ID3DXLine* p_Line;
	createLine(m_dxDevice, &p_Line);
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
	D3DCOLOR healthColor) const
{
	if (m_dxDevice == nullptr)
		return;

	box(x, y, width, height, px, color);

	ID3DXLine* p_LineHealth;
	createLine(m_dxDevice, &p_LineHealth);
	const FLOAT healthHeight = height - (health * height / 100);
	D3DXVECTOR2 linePos[] = { D3DXVECTOR2(x - 2.f, y + height), D3DXVECTOR2(x - 2.f, y + healthHeight) };
	p_LineHealth->SetWidth(1.f);
	p_LineHealth->SetAntialias(true);
	p_LineHealth->Draw(linePos, 2, healthColor);
	p_LineHealth->Release();
}

void draw_utils::string(const D3DXVECTOR2& pos, const std::string& text, D3DCOLOR color) const
{
	RECT fontPos = {
		static_cast<LONG>(pos.x),
		static_cast<LONG>(pos.y),
		static_cast<LONG>(pos.x) + 120,
		static_cast<LONG>(pos.y) + 16
	};
	m_dxFont->DrawText(nullptr, text.c_str(), -1, &fontPos, DT_NOCLIP, color);
}

void draw_utils::crosshair(DWORD size, D3DXVECTOR2 position, D3DCOLOR color) const
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

void draw_utils::render(void* ptr)
{
	m_dxDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_dxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_dxDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	m_dxDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0);
	m_dxDevice->BeginScene();

	hackProc(ptr);

	m_dxDevice->EndScene();
	HRESULT result = m_dxDevice->Present(nullptr, nullptr, nullptr, nullptr);
	if (result == D3DERR_DEVICELOST && m_dxDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		m_dxDevice->Reset(&d3dpp);
}
