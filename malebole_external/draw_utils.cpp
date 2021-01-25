#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3dx9math.h>
#include <string>
#include "draw_utils.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/VmpSdk.h"


typedef HRESULT(WINAPI* D3DXCreateLineFn)(LPDIRECT3DDEVICE9   pDevice, LPD3DXLINE* ppLine);

namespace draw_utils
{
	IDirect3DDevice9Ex* m_dxDevice;
	LPD3DXFONT m_dxFont;
	D3DPRESENT_PARAMETERS m_d3Params;
	HWND m_hWindow;
	RECT m_rDesktop;
	INT m_iWidth;
	INT m_iHeight;
}

void draw_utils::init_utils(HWND hWindow, RECT winRect)
{
	VM_START("draw_utils::draw_utils");
	m_dxDevice = nullptr;
	m_dxFont = nullptr;

	m_hWindow = hWindow;
	m_rDesktop = winRect;

	m_iHeight = m_rDesktop.bottom - m_rDesktop.top;
	m_iWidth = m_rDesktop.right - m_rDesktop.left;

	m_d3Params = {};

	IDirect3D9Ex* d3d = nullptr;
	Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);
	ZeroMemory(&m_d3Params, sizeof(D3DPRESENT_PARAMETERS));
	
	m_d3Params.Windowed = TRUE;
	m_d3Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_d3Params.EnableAutoDepthStencil = TRUE;
	m_d3Params.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3Params.PresentationInterval = 1;


	int vp;
	D3DCAPS9 devCaps;
	d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &devCaps);
	if (devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	d3d->CreateDeviceEx(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWindow,
		vp,
		&m_d3Params,
		nullptr,
		&m_dxDevice);

	D3DXCreateFontA(
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
		ENCRYPT_STR("Arial"),
		&m_dxFont);
	VM_END;
}

bool draw_utils::w2s(const D3DXVECTOR3 world, D3DXVECTOR2& screen, const float flMatrix[4][4])
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

void draw_utils::line(const D3DXVECTOR2& from, const D3DXVECTOR2& to, float width, D3DCOLOR color)
{
	if (m_dxDevice == nullptr)
		return;

	ID3DXLine* line;
	D3DXCreateLine(m_dxDevice, &line);
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
	D3DXCreateLine(m_dxDevice, &p_Line);
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
	D3DXCreateLine(m_dxDevice, &p_LineHealth);
	const FLOAT healthHeight = height - (health * height / 100);
	D3DXVECTOR2 linePos[] = { D3DXVECTOR2(x - 2.f, y + height), D3DXVECTOR2(x - 2.f, y + healthHeight) };
	p_LineHealth->SetWidth(1.f);
	p_LineHealth->SetAntialias(true);
	p_LineHealth->Draw(linePos, 2, healthColor);
	p_LineHealth->Release();
}

void draw_utils::string(const D3DXVECTOR2& pos, const std::string& text, D3DCOLOR color)
{
	RECT fontPos = {
		static_cast<LONG>(pos.x),
		static_cast<LONG>(pos.y),
		static_cast<LONG>(pos.x) + 120,
		static_cast<LONG>(pos.y) + 16
	};
	m_dxFont->DrawText(nullptr, text.c_str(), -1, &fontPos, DT_NOCLIP, color);
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

void draw_utils::render(void* ptr)
{
	m_dxDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_dxDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_dxDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	m_dxDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (m_dxDevice->BeginScene() >= 0)
	{
		hackProc(ptr);
		m_dxDevice->EndScene();
	}
	
	HRESULT result  = m_dxDevice->Present(0, 0, 0, 0);
	if (result == D3DERR_DEVICELOST && m_dxDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		m_dxDevice->Reset(&m_d3Params);
}
