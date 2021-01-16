#pragma once

//NETVARS
#define m_iTeamNum		0xF4
#define m_iHealth		0x100
#define m_vecOrigin		0x138
#define m_bDormant		0xED
#define	m_BoneMatrix	0x26A8
#define m_vecPunch		0x302C

//Offsets
#define dwLocalPlayer	0xD8B2AC
#define dwEntityList	0x4DA2E74
#define dwViewMatrix	0x4D94774

extern RECT DESKTOP_RECT;
extern DWORD CLIENT_DLL;

__forceinline void decrypt(unsigned char* str, size_t str_len)
{
	for (size_t i = 0; i < str_len; i++)
		str[i] = static_cast<unsigned char>(str[i] ^ i);
}

typedef HRESULT (WINAPI* D3DXCreateLineFn)(LPDIRECT3DDEVICE9   pDevice,LPD3DXLINE* ppLine);

extern D3DXCreateLineFn CreateLine;