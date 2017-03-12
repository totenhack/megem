#include "stdafx.h"

bool godmode = false;
bool kickglitch = false;
bool strang = false;

bool floatmode = false;
struct {
	float x, y, z, vx, vy, vz;
	float max_speed = 25;
	bool increase = false, decrease = false;
} float_data;

int engine_base;
int loading_addr;

void(*GetEngineBaseOriginal)();
int(__thiscall *UpdateEngineOriginal)(int this_);
void(*GetFallEffectOriginal)();

int __fastcall UpdateEngineHook(int this_, void *idle_) {
	if (!GetEngineBase()) {
		return UpdateEngineOriginal(this_);
	}

	DATA *data = GetData();

	if (Loading()) {
		memset(data, 0, sizeof(DATA));
	} else {
		int base = GetPlayerBase();
		if (base) {
			data->player_base = base;

			data->state = GetByte((void *)(base + PLAYER_STATE));
			data->x = GetFloat((void *)(base + PLAYER_X));
			data->y = GetFloat((void *)(base + PLAYER_Y));
			data->z = GetFloat((void *)(base + PLAYER_Z));
			data->vx = GetFloat((void *)(base + PLAYER_VX));
			data->vy = GetFloat((void *)(base + PLAYER_VY));
			data->vz = GetFloat((void *)(base + PLAYER_VZ));
			data->lx = GetFloat((void *)(base + PLAYER_LX));
			data->ly = GetFloat((void *)(base + PLAYER_LY));
			data->lz = GetFloat((void *)(base + PLAYER_LZ));
			data->body_rx = IntToDegrees(GetInt((void *)(base + PLAYER_RX)));
			data->body_ry = IntToDegrees(GetInt((void *)(base + PLAYER_RY)));
			data->body_rz = IntToDegrees(GetInt((void *)(base + PLAYER_RZ)));
			data->scale = GetFloat((void *)(base + PLAYER_SCALE));
			data->sx = GetFloat((void *)(base + PLAYER_SX));
			data->sy = GetFloat((void *)(base + PLAYER_SY));
			data->sz = GetFloat((void *)(base + PLAYER_SZ));
			data->health = GetInt((void *)(base + PLAYER_HEALTH));
		}

		base = GetCameraBase();
		if (base) {
			data->camera_base = base;

			data->rx = IntToDegrees(GetInt((void *)(base + CAMERA_RX)));
			data->ry = IntToDegrees(GetInt((void *)(base + CAMERA_RY)));
			data->rz = IntToDegrees(GetInt((void *)(base + CAMERA_RZ)));
		}

		base = GetEngineBase();
		if (base) {
			data->engine_base = base;

			data->speed = GetFloat((void *)(base + ENGINE_SPEED));
		}

		data->fov = GetFOV();
	}

	if (Loading() || *(int *)(GetEngineBase() + ENGINE_PAUSED) != 0) {
		return UpdateEngineOriginal(this_);
	}
	
	UpdateDolly();

	if (kickglitch && GetPlayerBase()) {
		char state = *(char *)(GetPlayerBase() + PLAYER_STATE);

		if (state == 1) {
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYDOWN, VK_SPACE, VK_SPACE);
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYUP, VK_SPACE, VK_SPACE);

			kickglitch = false;
		} else if (state == 12) {
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_LBUTTONDOWN, MK_LBUTTON, MK_LBUTTON);
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_LBUTTONUP, MK_LBUTTON, MK_LBUTTON);
		}
	}

	if (strang && GetPlayerBase()) {
		*(float *)(GetPlayerBase() + PLAYER_MAX_GROUND_SPEED) = 2147483648.0;

		if (*(char *)(GetPlayerBase() + PLAYER_STATE) == 1) {
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYDOWN, VK_SPACE, VK_SPACE);
			SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYUP, VK_SPACE, VK_SPACE);

			*(float *)(GetPlayerBase() + PLAYER_MAX_GROUND_SPEED) = 720;

			strang = false;
		}
	}

	if (floatmode && GetPlayerBase()) {
		float r = GetData()->ry;
		if (IsGameWindow(GetForegroundWindow())) {
			float hv = 0;
			float vv = 0;

			bool forward = GetAsyncKeyState(0x57) < 0;
			bool backward = GetAsyncKeyState(0x53) < 0;
			bool left = GetAsyncKeyState(0x41) < 0;
			bool right = GetAsyncKeyState(0x44) < 0;
			bool up = GetAsyncKeyState(VK_SPACE) < 0;
			bool down = GetAsyncKeyState(VK_SHIFT) < 0;

			if (backward) r += 180;
			if (left) r -= backward ? -45 : forward ? 45 : 90;
			if (right) r += backward ? -45 : forward ? 45 : 90;

			if (left || right || forward || backward) {
				hv = (float)(float_data.max_speed * 0.1);
			}

			if ((forward && backward) || (left && right)) {
				hv = 0;
			}

			if (up) {
				vv = (float)(float_data.max_speed * 0.1);
			} else if (down) {
				vv = -(float)(float_data.max_speed * 0.1);
			}

			float_data.vx += (float)cos(r * PI / 180) * hv;
			float_data.vy += (float)sin(r * PI / 180) * hv;
			float_data.vz += vv;

			if (float_data.increase) float_data.max_speed += 2;
			if (float_data.decrease) float_data.max_speed -= 2;

			if (float_data.max_speed < 1) {
				float_data.max_speed = 1;
			} else if (float_data.max_speed > 1000) {
				float_data.max_speed = 1000;
			}
		}

		float_data.x += float_data.vx;
		float_data.y += float_data.vy;
		float_data.z += float_data.vz;

		*(float *)(GetPlayerBase() + PLAYER_X) = float_data.x;
		*(float *)(GetPlayerBase() + PLAYER_Y) = float_data.y;
		*(float *)(GetPlayerBase() + PLAYER_Z) = float_data.z;

		float hs = (float)sqrt(float_data.vx * float_data.vx + float_data.vy * float_data.vy);
		hs *= hs;
		*(float *)(GetPlayerBase() + PLAYER_VX) = (float)cos(r * PI / 180) * hs;
		*(float *)(GetPlayerBase() + PLAYER_VY) = (float)sin(r * PI / 180) * hs;
		*(float *)(GetPlayerBase() + PLAYER_VZ) = SIGN(float_data.vz) * (float_data.vz * float_data.vz);

		*(char *)(GetPlayerBase() + PLAYER_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) = 11;
		*(char *)(GetPlayerBase() + PLAYER_WALKING_STATE) = 5;
		*(char *)(GetPlayerBase() + PLAYER_ACTION_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_HAND_STATE) = 0;
		*(int *)(GetPlayerBase() + PLAYER_HEALTH) = 100;
		*(float *)(GetPlayerBase() + PLAYER_FY) = -2147483648.0;

		float_data.vx *= (float)0.9;
		float_data.vy *= (float)0.9;
		float_data.vz *= (float)0.9;
	}

	if (godmode && GetPlayerBase()) {
		*(int *)(GetPlayerBase() + PLAYER_HEALTH) = 100;
		*(float *)(GetPlayerBase() + PLAYER_FY) = -2147483648.0;

		if (*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) == 20) {
			*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) = 1;
			*(char *)(GetPlayerBase() + PLAYER_WALKING_STATE) = 0;
			*(char *)(GetPlayerBase() + PLAYER_ACTION_STATE) = 0;
			*(char *)(GetPlayerBase() + PLAYER_HAND_STATE) = 0;
		}
	}

	return UpdateEngineOriginal(this_);
}

bool Loading() {
	return *(int *)loading_addr == 0;
}

void SetFloatIncrease(bool a) {
	float_data.increase = a;
}

void SetFloatDecrease(bool a) {
	float_data.decrease = a;
}

__declspec(naked) void GetEngineBaseHook() {
	__asm {
		mov engine_base, ecx
		jmp GetEngineBaseOriginal
	}
}

int GetEngineBase() {
	return engine_base;
}

void ToggleGodMode(bool a) {
	godmode = a;

	if (godmode && GetPlayerBase() && *(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) == 72) {
		*(char *)(GetPlayerBase() + PLAYER_STATE) = 2;
		*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) = 1;
		*(char *)(GetPlayerBase() + PLAYER_WALKING_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_ACTION_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_HAND_STATE) = 0;
	}
}

void KickGlitch(bool a) {
	kickglitch = a;
}

void Strang(bool a) {
	strang = a;
}

void ToggleFloatMode(bool a) {
	if (a) {
		if (GetPlayerBase()) {
			float_data.max_speed = 25;
			float_data.vx = float_data.vy = float_data.vz = 0;
			float_data.x = *(float *)(GetPlayerBase() + PLAYER_X);
			float_data.y = *(float *)(GetPlayerBase() + PLAYER_Y);
			float_data.z = *(float *)(GetPlayerBase() + PLAYER_Z);

			if (*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) == 72) {
				*(char *)(GetPlayerBase() + PLAYER_STATE) = 2;
				*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) = 1;
				*(char *)(GetPlayerBase() + PLAYER_WALKING_STATE) = 0;
				*(char *)(GetPlayerBase() + PLAYER_ACTION_STATE) = 0;
				*(char *)(GetPlayerBase() + PLAYER_HAND_STATE) = 0;
			}
		}
	} else if (GetPlayerBase()) {
		*(char *)(GetPlayerBase() + PLAYER_STATE) = 2;
		*(char *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE) = 1;
		*(char *)(GetPlayerBase() + PLAYER_WALKING_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_ACTION_STATE) = 0;
		*(char *)(GetPlayerBase() + PLAYER_HAND_STATE) = 0;
	}

	floatmode = a;
}

void SetupEngine() {
	int addr;

	// 0x11C4D99
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\xD9\xEE\x8B\x01\x8B\x50\x1C\x6A\x00\x6A\x01\x51\xD9\x1C\x24\xFF\xD2\x83\xA6\xF4\x04\x00\x00\xFA\xC7\x05", "xxxxxxxxxxxxxxxxxxxxxxxxxx");
	loading_addr = *(int *)(addr + 26);

	// addr = 0xEF90C0;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\x56\x8B\xF1\x8B\x06\x8B\x90\x34\x01\x00\x00\xFF\xD2\xD8\x8E\xD8\x0C\x00\x00\x5E", "xxxxxxxxxxxxxxxxxxxx");
	GetEngineBaseOriginal = (void(*)())NewGate(0);
	StaticTrampolineHook((void *)addr, GetEngineBaseHook, GetEngineBaseOriginal, 0);

	// addr = 0x1267670;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\x81\xEC\x8C\x00\x00\x00\x53\x56\x8B\xF1\x8B\x8E\xCC\x01\x00\x00\x8B\x81\xD0\x02\x00\x00\x8B\x90\xF8\x04\x00\x00\x05\xF8\x04\x00\x00\x89\x54\x24\x08\x8B\x50\x04\x89\x54\x24\x0C\x8B\x40\x08\x89\x44\x24\x10\x8B\x81\xD0\x02\x00\x00\x8B\x88\x04\x05\x00\x00\x05\x04\x05\x00\x00\x89\x4C\x24\x6C\x8B\x50\x04\x57\x8D\x4C\x24\x64\x89\x54\x24\x74\x8B\x40\x08\x51\x8D\x4C\x24\x74\x89\x44\x24\x7C", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	UpdateEngineOriginal = (int(__thiscall *)(int))NewGate(1);
	StaticTrampolineHook((void *)addr, UpdateEngineHook, UpdateEngineOriginal, 1);
}