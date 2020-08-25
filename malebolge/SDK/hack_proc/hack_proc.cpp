#include "../globals.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "../draw_utils/draw_utils.h"
#include "../../ring0/KernelInterface.h"
#include "hack_proc.h"


DWORD32 pLocal;
DWORD32 localHp;
DWORD32 localTeam;
DWORD32 localCrosshairId;

DWORD32 pEntity;
DWORD32 entHp;
DWORD32 entTeam;
BOOL entDormant;

D3DXVECTOR3 entPos(0, 0, 0);
D3DXVECTOR2 entScreen(0, 0);

D3DXVECTOR3 entHead(0, 0, 0);
D3DXVECTOR2 entHeadScreen(0, 0);

D3DXVECTOR3 vecPunch(0, 0, 0);

float viewMatrix[4][4];

typedef struct GlowObjectDefinition_t {
	float r;
	float g;
	float b;
	float a;
	uint8_t unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	int GlowStyle;
	uint8_t unk2[10];
} GlowObjectDefinition_t;

D3DXVECTOR3 getBonePos(DWORD32 pTargetIn, int bone, KernelInterface* reader)
{
	DWORD32 pMatrix = 0;
	reader->Read32(pTargetIn + m_BoneMatrix, &pMatrix);
	if (!pMatrix)
		return D3DXVECTOR3(0, 0, 0);

	FLOAT px;
	FLOAT py;
	FLOAT pz;
	reader->Read32(pMatrix + 0x30 * bone + 0xC, &px);
	reader->Read32(pMatrix + 0x30 * bone + 0x1C, &py);
	reader->Read32(pMatrix + 0x30 * bone + 0x2C, &pz);

	return D3DXVECTOR3(px, py, pz);

}


void draw_utils::hackProc(void* ptr)
{
	KernelInterface* ring0 = static_cast<KernelInterface*>(ptr);

	ring0->Read32(ring0->Modules->bClient + dwLocalPlayer, &pLocal);
	if (!pLocal)
		return;

	ring0->Read32(pLocal + m_iTeamNum, &localTeam);
	if (localTeam != 2 && localTeam != 3)
		return;

	ring0->Read32(pLocal + m_aimPunchAngle, &vecPunch);
	crosshair(10, 
		D3DXVECTOR2(
			static_cast<float>(m_iWidth) / 2.f - (static_cast<float>(m_iWidth) / 90 * vecPunch.y), 
			static_cast<float>(m_iHeight / 2.f) + (static_cast<float>(m_iHeight) / 90 * vecPunch.x)
		), 
		D3DCOLOR_ARGB(255, 0, 255, 0));
	for (DWORD32 i = 1; i < 64; i++)
	{
		ring0->Read32(ring0->Modules->bClient + dwEntityList + (i - 1) * 0x10, &pEntity);
		if (!pEntity)
			continue;

		ring0->Read32(pEntity + m_iHealth, &entHp);
		if (!entHp)
			continue;

		ring0->Read32(pEntity + m_iTeamNum, &entTeam);
		if (entTeam == localTeam)
			continue;


		ring0->Read32(pEntity + m_bDormant, &entDormant);
		if (entDormant)
			continue;

		ring0->Read32(ring0->Modules->bClient + mViewMatrix, &viewMatrix);
		ring0->Read32(pEntity + m_vecOrigin, &entPos);
		if (!w2s(entPos, entScreen, viewMatrix))
			continue;

		entHead = getBonePos(pEntity, 8, ring0);
		if (entHead.x == 0 && entHead.y == 0 && entHead.z == 0.f)
			continue;

		if (!w2s(entHead, entHeadScreen, viewMatrix))
			continue;

		FLOAT boxHeight = abs(entHeadScreen.y - entScreen.y);
		FLOAT boxWidth = boxHeight / 2;
		healthBox(entScreen.x - boxWidth / 2, entHeadScreen.y, boxWidth, boxHeight, 2.f, entHp, D3DCOLOR_ARGB(255, 255, 0, 0), D3DCOLOR_ARGB(255, 0, 255, 0));
		string(D3DXVECTOR2(entScreen.x, entScreen.y), std::to_string(entHp), D3DCOLOR_ARGB(255, 255, 255, 0));
	}
}