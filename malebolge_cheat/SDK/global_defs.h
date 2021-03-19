#pragma once

//NETVARS
#define m_iTeamNum		0xF4
#define m_iHealth		0x100
#define m_vecOrigin		0x138
#define m_bDormant		0xED
#define	m_BoneMatrix	0x26A8
#define m_vecPunch		0x302C

//Offsets
#define dwLocalPlayer	0xD8B2BC
#define dwEntityList	0x4DA2F54
#define dwViewMatrix	0x4D94854

extern RECT DESKTOP_RECT;
extern DWORD CLIENT_DLL;

typedef HRESULT (WINAPI* D3DXCreateLineFn)(LPDIRECT3DDEVICE9   pDevice,LPD3DXLINE* ppLine);

extern D3DXCreateLineFn CreateLine;