#pragma once

//Driver & Game Name
/**
 * \brief Driver device Name
 */
#define DRIVER_NAME						"\\\\.\\fgjvmeo24"

/**
* \brief Game for hacking
*/
#define GAME_NAME						"csgo.exe"



//Offsets
extern DWORD32 dwLocalPlayer;
extern DWORD32 dwEntityList;
extern DWORD32 mViewMatrix;

extern DWORD32 m_vecOrigin;
extern DWORD32 m_iHealth;
extern DWORD32 m_bDormant;
extern DWORD32 m_iTeamNum;
extern DWORD32 m_BoneMatrix;
extern DWORD32 m_aimPunchAngle;
