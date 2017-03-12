#pragma once

#include "targetver.h"

#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <tlhelp32.h>
#include <commctrl.h>
#include <sys/stat.h>

#include "Megem.h"
#include "visual.h"

#define WIDTH 225
#define HEIGHT 350

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

#define LENGTH(a) (sizeof(a) / sizeof(a[0]))

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

static int STATE_PLAYER_DATA[] = { 0x68, 0x4FE, 0x503, 0x505, 0x4F4, 0x2A0, 0x2A4, 0x114, 0x2A8, 0xE8, 0xEC, 0xF0, 0x100, 0x104, 0x108, 0xF4, 0xF8, 0xFC, 0x264, 0x2B8, 0x2E0, 0x2E4, 0x2E8, 0x5CC, 0x5D0, 0x5D4, 0x4CC, 0x4D0, 0x4D4, 0x72C };
static int STATE_CAMERA_DATA[] = { 0xF4, 0xF8, 0xFC, 0x5A8 };

typedef struct {
	int player_data[LENGTH(STATE_PLAYER_DATA)];
	int camera_data[LENGTH(STATE_CAMERA_DATA)];
} STATE;

void SaveState(STATE *state);
void LoadState(STATE *state);

typedef struct {
	char *name;
	int keycode;
} KEY;

static KEY KEYS[] = {
	{ "lmb", 0x01 },
	{ "leftmousebutton", 0x01 },
	{ "left_mouse_button", 0x01 },
	{ "rmb", 0x02 },
	{ "rightmousebutton", 0x02 },
	{ "right_mouse_button", 0x02 },
	{ "cancel", 0x03 },
	{ "mmb", 0x04 },
	{ "middlemousebutton", 0x04 },
	{ "middle_mouse_button", 0x04 },
	{ "x1mb", 0x05 },
	{ "x1mousebutton", 0x05 },
	{ "x1_mouse_button", 0x05 },
	{ "x2mb", 0x06 },
	{ "x2mousebutton", 0x06 },
	{ "x2_mouse_button", 0x06 },
	{ "undefined", 0x07 },
	{ "backspace", 0x08 },
	{ "back", 0x08 },
	{ "tab", 0x09 },
	{ "clear", 0x0C },
	{ "return", 0x0D },
	{ "enter", 0x0D },
	{ "shift", 0x10 },
	{ "control", 0x11 },
	{ "ctrl", 0x11 },
	{ "menu", 0x12 },
	{ "alt", 0x12 },
	{ "pause", 0x13 },
	{ "capital", 0x14 },
	{ "capslock", 0x14 },
	{ "caps", 0x14 },
	{ "caps_lock", 0x14 },
	{ "kana", 0x15 },
	{ "hanguel", 0x15 },
	{ "junja", 0x17 },
	{ "final", 0x18 },
	{ "hanja", 0x19 },
	{ "kanji", 0x19 },
	{ "escape", 0x1B },
	{ "esc", 0x1B },
	{ "convert", 0x1C },
	{ "nonconvert", 0x1D },
	{ "accept", 0x1E },
	{ "modechange", 0x1F },
	{ "mode_change", 0x1F },
	{ "space", 0x20 },
	{ "spacebar", 0x20 },
	{ "pageup", 0x21 },
	{ "page_up", 0x21 },
	{ "pagedown", 0x22 },
	{ "page_down", 0x22 },
	{ "end", 0x23 },
	{ "home", 0x24 },
	{ "left", 0x25 },
	{ "left_arrow", 0x25 },
	{ "leftarrow", 0x25 },
	{ "up", 0x26 },
	{ "up_arrow", 0x26 },
	{ "uparrow", 0x26 },
	{ "right", 0x27 },
	{ "right_arrow", 0x27 },
	{ "rightarrow", 0x27 },
	{ "down", 0x28 },
	{ "down_arrow", 0x28 },
	{ "downarrow", 0x28 },
	{ "select", 0x29 },
	{ "print", 0x2A },
	{ "execute", 0x2B },
	{ "printscreen", 0x2C },
	{ "prtscr", 0x2C },
	{ "print_screen", 0x2C },
	{ "insert", 0x2D },
	{ "delete", 0x2E },
	{ "help", 0x2F },
	{ "0", 0x30 },
	{ "1", 0x31 },
	{ "2", 0x32 },
	{ "3", 0x33 },
	{ "4", 0x34 },
	{ "5", 0x35 },
	{ "6", 0x36 },
	{ "7", 0x37 },
	{ "8", 0x38 },
	{ "9", 0x39 },
	{ "a", 0x41 },
	{ "b", 0x42 },
	{ "c", 0x43 },
	{ "d", 0x44 },
	{ "e", 0x45 },
	{ "f", 0x46 },
	{ "g", 0x47 },
	{ "h", 0x48 },
	{ "i", 0x49 },
	{ "j", 0x4A },
	{ "k", 0x4B },
	{ "l", 0x4C },
	{ "m", 0x4D },
	{ "n", 0x4E },
	{ "o", 0x4F },
	{ "p", 0x50 },
	{ "q", 0x51 },
	{ "r", 0x52 },
	{ "s", 0x53 },
	{ "t", 0x54 },
	{ "u", 0x55 },
	{ "v", 0x56 },
	{ "w", 0x57 },
	{ "x", 0x58 },
	{ "y", 0x59 },
	{ "z", 0x5A },
	{ "lwin", 0x5B },
	{ "leftwindowskey", 0x5B },
	{ "left_windows_key", 0x5B },
	{ "rwin", 0x5C },
	{ "rightwindowskey", 0x5C },
	{ "right_windows_key", 0x5C },
	{ "apps", 0x5D },
	{ "sleep", 0x5F },
	{ "numpad0", 0x60 },
	{ "numpad1", 0x61 },
	{ "numpad2", 0x62 },
	{ "numpad3", 0x63 },
	{ "numpad4", 0x64 },
	{ "numpad5", 0x65 },
	{ "numpad6", 0x66 },
	{ "numpad7", 0x67 },
	{ "numpad8", 0x68 },
	{ "numpad9", 0x69 },
	{ "multiply", 0x6A },
	{ "add", 0x6B },
	{ "separator", 0x6C },
	{ "subtract", 0x6D },
	{ "decimal", 0x6E },
	{ "divide", 0x6F },
	{ "f1", 0x70 },
	{ "f2", 0x71 },
	{ "f3", 0x72 },
	{ "f4", 0x73 },
	{ "f5", 0x74 },
	{ "f6", 0x75 },
	{ "f7", 0x76 },
	{ "f8", 0x77 },
	{ "f9", 0x78 },
	{ "f10", 0x79 },
	{ "f11", 0x7A },
	{ "f12", 0x7B },
	{ "f13", 0x7C },
	{ "f14", 0x7D },
	{ "f15", 0x7E },
	{ "f16", 0x7F },
	{ "f17", 0x80 },
	{ "f18", 0x81 },
	{ "f19", 0x82 },
	{ "f20", 0x83 },
	{ "f21", 0x84 },
	{ "f22", 0x85 },
	{ "f23", 0x86 },
	{ "f24", 0x87 },
	{ "numberlock", 0x90 },
	{ "number_lock", 0x90 },
	{ "numlock", 0x90 },
	{ "num_lock", 0x90 },
	{ "scroll", 0x91 },
	{ "lshift", 0xA0 },
	{ "leftshift", 0xA0 },
	{ "left_shift", 0xA0 },
	{ "rshift", 0xA1 },
	{ "rightshift", 0xA1 },
	{ "right_shift", 0xA1 },
	{ "lcontrol", 0xA2 },
	{ "leftcontrol", 0xA2 },
	{ "left_control", 0xA2 },
	{ "rcontrol", 0xA3 },
	{ "rightcontrol", 0xA3 },
	{ "right_control", 0xA3 },
	{ "lmenu", 0xA4 },
	{ "leftmenu", 0xA4 },
	{ "left_menu", 0xA4 },
	{ "rmenu", 0xA5 },
	{ "rightmenu", 0xA5 },
	{ "right_menu", 0xA5 },
	{ "+", 0xBB },
	{ "plus", 0xBB },
	{ ",", 0xBC },
	{ "comma", 0xBC },
	{ "-", 0xBD },
	{ "minus", 0xBD },
	{ ".", 0xBE },
	{ "period", 0xBE }
};

int IsGameWindow(HWND hWnd);
void SetupKeybinds();
void MainThread();
LRESULT CALLBACK KeyboardHookProc(int arg, WPARAM wParam, LPARAM lParam);
int getIndexByKeyCode(int keycode);
HANDLE GetProcess();
HANDLE CallFunction(char *name, void *argument);
DATA *GetData();
HWND GetHWND();
int ProcessHasModule(int pid, wchar_t *module);
int ModuleBase(int pid, wchar_t *module);
void AppendItemToTree(HWND tree, char *text);
int GetTreeSelectedIndex(HWND tree);
void prepend(char *dest, char *src);
void SuspendCurrentProcess();
void ResumeCurrentProcess();
float IntToDegrees(int int_);
int DegreesToInt(float degrees);
int FileExists(char *path);
int GetProcessCount(TCHAR *process);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK FOVProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK KeybindsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VisualPropertyProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ProcessSpeedProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK EngineSpeedProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK LevelStreamerProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DollyCameraProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VisualPropertiesProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PositionXProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PositionYProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PositionZProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VelocityXProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VelocityYProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VelocityZProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RotationXProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RotationYProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RotationZProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ScaleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ScaleXProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ScaleYProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ScaleZProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
