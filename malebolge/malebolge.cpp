#include <Windows.h>
#include "SDK/globals.h"
#include "ring0/KernelInterface.h"

KernelInterface ring0;

typedef struct GlowObjectDefinition_t {
	float r;
	float g;
	float b;
	float a;
	UINT8 unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	int GlowStyle;
	UINT8 unk2[10];
} GlowObjectDefinition_t;

#define dwLocalPlayer				0xD3FC5C
#define dwEntityList				0x4D5450C
#define dwGlowObject				0x529C3D0

#define m_iTeamNum					0xF4
#define m_iHealth 					0x100

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow)
{
	return 0;
}