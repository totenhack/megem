#pragma once

#include "targetver.h"

#include <stdio.h>
#include <math.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "winmm")
#pragma comment(lib, "ws2_32.lib")

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "spline.h"

#define EXPORT __declspec(dllexport)
#define FREE(a) VirtualFree(a, 0, MEM_RELEASE)
#define LENGTH(a) sizeof(a) / sizeof(a[0])
#define PI 3.141592653589793
#define SIGN(a) (a < 0 ? -1 : 1)

#define PLAYER_STATE 0x68
#define PLAYER_MOVEMENT_STATE 0x4FE
#define PLAYER_WALKING_STATE 0x503
#define PLAYER_ACTION_STATE 0x505
#define PLAYER_HAND_STATE 0x4F4
#define PLAYER_WALLRUN_STATE 0x2A0
#define PLAYER_WALL_VZ 0x114
#define PLAYER_WALL_STATE 0x2A8
#define PLAYER_X 0xE8
#define PLAYER_Y 0xEC
#define PLAYER_Z 0xF0
#define PLAYER_VX 0x100
#define PLAYER_VY 0x104
#define PLAYER_VZ 0x108
#define PLAYER_RX 0xF4
#define PLAYER_RY 0xF8
#define PLAYER_RZ 0xFC
#define PLAYER_SCALE 0x154
#define PLAYER_SX 0x158
#define PLAYER_SY 0x15C
#define PLAYER_SZ 0x160
#define PLAYER_MAX_GROUND_SPEED 0x264
#define PLAYER_HEALTH 0x2B8
#define PLAYER_LX 0x2E0
#define PLAYER_LY 0x2E4
#define PLAYER_LZ 0x2E8
#define PLAYER_OX 0x5CC
#define PLAYER_OY 0x5D0
#define PLAYER_OZ 0x5D4
#define PLAYER_PX 0x4CC
#define PLAYER_PY 0x4D0
#define PLAYER_PZ 0x4D4
#define PLAYER_FY 0x72C

#define CAMERA_RX 0xF4
#define CAMERA_RY 0xF8
#define CAMERA_RZ 0xFC
#define CAMERA_REACTION_TIME 0x5A8

#define ENGINE_SPEED 0xC2C
#define ENGINE_PAUSED 0xC54

typedef struct {
	char state;
	float x, y, z, vx, vy, vz, lx, ly, lz;
	float body_rx, body_ry, body_rz;
	float scale, sx, sy, sz;
	int health;
	float rx, ry, rz;
	float speed, process_speed, fov;

	int player_base, camera_base, engine_base;

	int sublevel_count;
	char *sublevels;
} DATA;

DATA *GetData();
int GetInt(void *address);
float GetFloat(void *address);
char GetByte(void *address);
int IsGameWindow(HWND hWnd);

extern "C" {
	#include "hook.h"
	#include "player.h"
	#include "speed.h"
	#include "render.h"
	#include "state.h"
	#include "dolly.h"
	#include "engine.h"
	#include "level.h"

	EXPORT void EXPORT_ResetDolly();
	EXPORT void EXPORT_SetDollyDOF(float *dof);
	EXPORT void EXPORT_SetDollyDuration(int *duration);
	EXPORT void EXPORT_AddDollyMarker();
	EXPORT void EXPORT_PlayDolly();
	EXPORT void EXPORT_EnableFloatIncrease();
	EXPORT void EXPORT_DisableFloatIncrease();
	EXPORT void EXPORT_EnableFloatDecrease();
	EXPORT void EXPORT_DisableFloatDecrease();
	EXPORT void EXPORT_Strang();
	EXPORT void EXPORT_StopStrang();
	EXPORT void EXPORT_KickGlitch();
	EXPORT void EXPORT_StopKickGlitch();
	EXPORT void EXPORT_EnableFloatMode();
	EXPORT void EXPORT_DisableFloatMode();
	EXPORT void EXPORT_EnableGodMode();
	EXPORT void EXPORT_DisableGodMode();
	EXPORT void EXPORT_GetSublevelStatus(char *name);
	EXPORT void EXPORT_GetSublevels();
	EXPORT void EXPORT_SetSpeed(float *speed_ptr);
	EXPORT void EXPORT_SetFOV(float *fov_ptr);
	EXPORT void EXPORT_SetDOF(float *dof_ptr);
	EXPORT void EXPORT_LoadLevelStreamByString(char *str);
	EXPORT void EXPORT_UnloadLevelStreamByString(char *str);
}