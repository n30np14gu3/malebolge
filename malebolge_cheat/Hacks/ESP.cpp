#pragma comment(lib, "d3dx9.lib")

#include <Windows.h>

#include <d3dx9.h>
#include <d3d9.h>

#include <string>


#include "../SDK/lazy_importer.hpp"
#include "../SDK/draw_utils.h"
#include "../SDK/global_defs.h"

DWORD pLocal;
DWORD localHp;
DWORD localTeam;

DWORD pEntity;
DWORD entHp;
DWORD entTeam;
BOOL entDormant;

D3DXVECTOR3 entPos(0, 0, 0);
D3DXVECTOR2 entScreen(0, 0);

D3DXVECTOR3 entHead(0, 0, 0);
D3DXVECTOR2 entHeadScreen(0, 0);

D3DXVECTOR3 vecPunch(0, 0, 0);
float* viewMatrix;

char sHp[10] = { 0 };

void my_reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end)
	{
		std::swap(*(str + start), *(str + end));
		start++;
		end--;
	}
}


char* my_itoa(int num, char* str, int base)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with  
	// base 10. Otherwise numbers are considered unsigned. 
	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

	// Process individual digits 
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-' 
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator 

	// Reverse the string 
	my_reverse(str, i);

	return str;
}
D3DXVECTOR3 getBonePos(DWORD pTargetIn, DWORD bone)
{
	DWORD pMatrix = *reinterpret_cast<DWORD*>(pTargetIn + m_BoneMatrix);
	if (!pMatrix)
		return D3DXVECTOR3(0, 0, 0);

	FLOAT px = *reinterpret_cast<FLOAT*>(pMatrix + 0x30 * bone + 0xC);
	FLOAT py = *reinterpret_cast<FLOAT*>(pMatrix + 0x30 * bone + 0x1C);
	FLOAT pz = *reinterpret_cast<FLOAT*>(pMatrix + 0x30 * bone + 0x2C);

	return D3DXVECTOR3(px, py, pz);

}

void render()
{
	pLocal = *reinterpret_cast<DWORD*>(CLIENT_DLL + dwLocalPlayer);
	if (!pLocal)
		return;

	localTeam = *reinterpret_cast<DWORD*>(pLocal + m_iTeamNum);

	vecPunch = *reinterpret_cast<D3DXVECTOR3*>(pLocal + m_vecPunch);

	draw_utils::crosshair(7,
		D3DXVECTOR2(
			static_cast<float>(draw_utils::m_iWidth) / 2.f - (static_cast<float>(draw_utils::m_iWidth) / 90 * vecPunch.y),
			static_cast<float>(draw_utils::m_iHeight) / 2.f + (static_cast<float>(draw_utils::m_iHeight) / 90 * vecPunch.x)
		),
		D3DCOLOR_ARGB(255, 0, 255, 0));

	viewMatrix = reinterpret_cast<float*>(CLIENT_DLL + dwViewMatrix);
	
	for (DWORD i = 1; i < 64; i++)
	{
		pEntity = *reinterpret_cast<DWORD*>(CLIENT_DLL + dwEntityList + (i - 1) * 0x10);
		if (!pEntity)
			continue;

		entTeam = *reinterpret_cast<DWORD*>(pEntity + m_iTeamNum);
		entDormant = *reinterpret_cast<BOOL*>(pEntity + m_bDormant);
		entHp = *reinterpret_cast<DWORD*>(pEntity + m_iHealth);


		if(!entHp || entTeam == localTeam || entDormant)
			continue;

		
		entPos = *reinterpret_cast<D3DXVECTOR3*>(pEntity + m_vecOrigin);		
		if (!draw_utils::w2s(entPos, entScreen, viewMatrix))
			continue;
		
		entHead = getBonePos(pEntity, 8);

		if (entHead.x == 0.f && entHead.y == 0.f && entHead.z == 0.f)
			continue;

		if (!draw_utils::w2s(entHead, entHeadScreen, viewMatrix))
			continue;

		FLOAT boxHeight = abs(entHeadScreen.y - entScreen.y);
		FLOAT boxWidth = boxHeight / 2;

		my_itoa(entHp, sHp, 10);
		draw_utils::healthBox(entScreen.x - boxWidth / 2, entHeadScreen.y, boxWidth, boxHeight, 2.f, entHp, D3DCOLOR_ARGB(255, 255, 0, 0), D3DCOLOR_ARGB(255, 0, 255, 0));
		draw_utils::string(D3DXVECTOR2(entScreen.x, entScreen.y), sHp, D3DCOLOR_ARGB(255, 255, 255, 0));

	}
}
