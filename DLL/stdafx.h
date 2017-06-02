#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <wctype.h>
#include <Shlobj.h>
#include <d3d9.h>
#include <d3dx9.h>

#define GetGameWindow() FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0)
#define PI 3.141592653589793
#define SIGN(a) (a < 0 ? -1 : 1)
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

typedef struct {
	DWORD player_base, camera_base, engine_base, dof_base, uncap_base;
	float top_speed;
	float fov;
	float process_speed;
	long long frame;
	char level[0xFF];
	int sublevel_count;
	char *sublevels;
	bool loading;
} DATA;

typedef struct {
	short pause, speed0, speed25, speed5, speed75, speed1, speed2, god, fly, fly_increase, fly_decrease, fly_forward, fly_backward, fly_left, fly_right, kickglitch, reactiontime, strang, beamer, quickturn, checkpoint, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, l0, l1, l2, l3, l4, l5, l6, l7, l8, l9;
} KEYBINDS;

DATA *GetData();
KEYBINDS *GetKeybinds();
int wcsempty(const wchar_t *s);
int IsGameWindow(HWND hWnd);
void Keydown(SHORT k);
void Keyup(SHORT k);

#define EXPORT __declspec(dllexport)

extern "C" {
	#include "memory.h"
	#include "spline.h"
	#include "speed.h"
	#include "player.h"
	#include "engine.h"
	#include "render.h"
	#include "level.h"
	#include "visual.h"
	#include "dolly.h"

	EXPORT void EXPORT_GetDataBase(DWORD *out);
	EXPORT void EXPORT_SetProcessSpeed(float *speed);
	EXPORT void EXPORT_EnableRendering();
	EXPORT void EXPORT_DisableRendering();
	EXPORT void EXPORT_KG();
	EXPORT void EXPORT_Strang();
	EXPORT void EXPORT_Beamer();
	EXPORT void EXPORT_GodOn();
	EXPORT void EXPORT_GodOff();
	EXPORT void EXPORT_FloatOn();
	EXPORT void EXPORT_FloatOff();
	EXPORT void EXPORT_OpenDebugConsole();
	EXPORT void EXPORT_CloseDebugConsole();
	EXPORT void EXPORT_DisableAll();
	EXPORT void EXPORT_LoadLevelStreamByString(char *str);
	EXPORT void EXPORT_UnloadLevelStreamByString(char *str);
	EXPORT void EXPORT_GetSublevels();
	EXPORT void EXPORT_GetSublevelStatus(char *str);
	EXPORT void EXPORT_SetFOV(float *fov);
	EXPORT void EXPORT_AddDollyMarker(DWORD *frame);
	EXPORT void EXPORT_ResetDolly();
	EXPORT void EXPORT_PlayDolly();
}
