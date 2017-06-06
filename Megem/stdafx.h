#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <TlHelp32.h>
#include <ctype.h>
#include <Commctrl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Resource.h"
#include "memory.h"
#include "visual.h"

#pragma comment(lib, "Winmm.lib")

#define WIDTH 235
#define HEIGHT 350
#define PI 3.141592653589793
#define GetGameWindow() FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0)
#define PromptProc(name) INT_PTR CALLBACK name(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
#define CreatePrompt(id, proc) ShowWindow(CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(id), hWnd, proc), SW_SHOW)
#define LENGTH(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
	DWORD player_base, camera_base, engine_base, dof_base;
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

typedef struct {
	DWORD color_background, color_text, color_active;
	bool toggle_fov, toggle_vp;
	float fov;
	DWORD visual_properties[LENGTH(visual_properties)];
} SETTINGS;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
SETTINGS *GetSettings();
KEYBINDS *GetKeybinds();
int IsGameWindow(HWND hWnd);
HWND GetCurrentWindow();
bool FlyOn();
DATA GetData();
HANDLE GetProcess();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ProcessListener();
HANDLE CallFunction(char *name, void *arg);
float IntToDegrees(int i);
int DegreesToInt(float i);
void prepend(char *dest, char *src);
void Keydown(SHORT k);
void Keyup(SHORT k);
DWORD GetFileSize(char *path);

#define PLAYER_COLLISION 0x40
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
#define PLAYER_MAX_HEALTH 0x2BC
#define PLAYER_LGX 0x2E0
#define PLAYER_LGY 0x2E4
#define PLAYER_LGZ 0x2E8
#define PLAYER_OX 0x5CC
#define PLAYER_OY 0x5D0
#define PLAYER_OZ 0x5D4
#define PLAYER_PX 0x4CC
#define PLAYER_PY 0x4D0
#define PLAYER_PZ 0x4D4
#define PLAYER_FZ 0x72C

#define CAMERA_RX 0xF4
#define CAMERA_RY 0xF8
#define CAMERA_RZ 0xFC
#define CAMERA_LOCK 0x2FC
#define CAMERA_REACTION_TIME 0x5A8

#define ENGINE_SPEED 0xC2C
#define ENGINE_PAUSED 0xC54

#include "dialogs.h"
#include "state.h"
#include "slew.h"