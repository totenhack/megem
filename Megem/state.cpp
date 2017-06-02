#include "stdafx.h"

STATE SaveState() {
	STATE state;

	if (GetProcess()) {
		DATA data = GetData();
		ReadProcessMemory(GetProcess(), (void *)data.player_base, state.player, sizeof(state.player), NULL);
		ReadProcessMemory(GetProcess(), (void *)data.camera_base, state.camera, sizeof(state.camera), NULL);
		strcpy(state.level, data.level);
	} else {
		memset(state.player, 0, sizeof(state.player));
		memset(state.camera, 0, sizeof(state.camera));
	}

	return state;
}

// DWORD player_offsets[] = { 0x40, 0x44, 0x68, 0xE8, 0xEC, 0xF0, 0xF4, 0xF8, 0xFC, 0x100, 0x104, 0x108, 0x10C, 0x110, 0x114, 0x154, 0x158, 0x15C, 0x160, 0x164, 0x168, 0x16C, 0x1C0, 0x1C8, 0x1DC, 0x1E4, 0x1FC, 0x20C, 0x264, 0x2A0, 0x2A4, 0x2A8, 0x2AC, 0x2B8, 0x2E0, 0x2E4, 0x2E8, 0x3B4, 0x428, 0x4CC, 0x4D0, 0x4D4, 0x4F4, 0x4FC, 0x4FE, 0x500, 0x503, 0x505, 0x57F, 0x5D0, 0x5D4, 0x610, 0x614, 0x630, 0x634, 0x6F8, 0x6FC, 0x700, 0x704, 0x708, 0x70C, 0x72C };
// DWORD camera_offsets[] = { 0xF4, 0xF8, 0xFC, 0x254, 0x258, 0x25C, 0x260, 0x264, 0x268, 0x26C, 0x270, 0x530, 0x58C, 0x5E0, 0x638, 0x63C }; */

DWORD player_offsets[] = { PLAYER_X, PLAYER_Y, PLAYER_Z, PLAYER_VX, PLAYER_VY, PLAYER_VZ, PLAYER_RX, PLAYER_RY, PLAYER_RZ, PLAYER_STATE, PLAYER_WALL_STATE, PLAYER_WALL_VZ, PLAYER_WALLRUN_STATE, PLAYER_WALLRUN_STATE + 4, 0x630, PLAYER_MOVEMENT_STATE, PLAYER_ACTION_STATE, PLAYER_WALKING_STATE, PLAYER_HEALTH, PLAYER_FY };
DWORD camera_offsets[] = { CAMERA_RX, CAMERA_RY, CAMERA_RZ };

void LoadState(STATE *state) {
	if (GetProcess()) {
		DATA data = GetData();

		if (strcmp(data.level, state->level) != 0) {
			SetForegroundWindow(GetCurrentWindow());
			SetActiveWindow(GetCurrentWindow());
			if (MessageBoxA(0, "The state you are attempting to load was saved on a different level and may cause unintended results. Do you want to load it?", "Load Conflict", MB_YESNO | MB_TOPMOST) != IDYES) {
				return;
			}
		}

		WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_COLLISION), -105.578125);
		WriteInt(GetProcess(), (void *)(data.camera_base + CAMERA_LOCK), 0);
		if (*(char *)(state->player + PLAYER_STATE) == 12) {
			CallFunction("EXPORT_DisableRendering", 0);

			WriteBuffer(GetProcess(), (void *)(data.player_base + PLAYER_X), (char *)(state->player + PLAYER_X), sizeof(float) * 3);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VX), 0);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VY), 0);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VZ), 0);

			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_MOVEMENT_STATE), 23);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_HAND_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_WALKING_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_ACTION_STATE), 0);

			long long frame = GetData().frame + 1;
			while (GetData().frame <= frame) {
				Sleep(1);
			}

			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_STATE), 2);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALLRUN_STATE), 0);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALLRUN_STATE + 4), 0);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_WALL_STATE), 1);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALL_VZ), 1);

			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_MOVEMENT_STATE), 1);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_HAND_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_WALKING_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_ACTION_STATE), 0);

			frame = GetData().frame + 1;
			while (GetData().frame <= frame) {
				Sleep(1);
			}

			WriteBuffer(GetProcess(), (void *)(data.player_base + PLAYER_X), (char *)(state->player + PLAYER_X), sizeof(float) * 3);
			DWORD angle = 0;
			if (*(DWORD *)(state->player + 0x610) == 17) { // Left side
				WriteInt(GetProcess(), (void *)(data.camera_base + CAMERA_RY), angle = *(DWORD *)(state->camera + CAMERA_RY) - (DWORD)(.1 * 0x10000));
			} else {
				WriteInt(GetProcess(), (void *)(data.camera_base + CAMERA_RY), angle = *(DWORD *)(state->camera + CAMERA_RY) + (DWORD)(.1 * 0x10000));
			}
			float vx = *(float *)(state->player + PLAYER_VX);
			float vy = *(float *)(state->player + PLAYER_VY);
			float speed = (float)sqrt(vx * vx + vy * vy);
			vx = (float)cos(IntToDegrees(angle) * (PI / 180)) * speed;
			vy = (float)sin(IntToDegrees(angle) * (PI / 180)) * speed;
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_STATE), 2);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_MOVEMENT_STATE), 11);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_HAND_STATE), 0);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_WALKING_STATE), 5);
			WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_ACTION_STATE), 0);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALLRUN_STATE), 0);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALLRUN_STATE + 4), 0);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_WALL_STATE), 1);
			WriteInt(GetProcess(), (void *)(data.player_base + PLAYER_WALL_VZ), 0);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VX), vx);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VY), vy);
			WriteFloat(GetProcess(), (void *)(data.player_base + PLAYER_VZ), (float)552.5698242);

			frame = GetData().frame + 1;
			while (GetData().frame <= frame) {
				Sleep(1);
			}

			CallFunction("EXPORT_EnableRendering", 0);

			for (int i = 0; i < sizeof(player_offsets) / sizeof(DWORD); ++i) {
				WriteInt(GetProcess(), (void *)(data.player_base + player_offsets[i]), *(DWORD *)(state->player + player_offsets[i]));
			}

			for (int i = 0; i < sizeof(camera_offsets) / sizeof(DWORD); ++i) {
				WriteInt(GetProcess(), (void *)(data.camera_base + camera_offsets[i]), *(DWORD *)(state->camera + camera_offsets[i]));
			}
		} else {
			for (int i = 0; i < sizeof(player_offsets) / sizeof(DWORD); ++i) {
				WriteInt(GetProcess(), (void *)(data.player_base + player_offsets[i]), *(DWORD *)(state->player + player_offsets[i]));
			}

			if (*(char *)(state->player + PLAYER_STATE) == 1) {
				WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_STATE), 2);
				WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_MOVEMENT_STATE), 11);
				WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_HAND_STATE), 0);
				WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_WALKING_STATE), 5);
				WriteChar(GetProcess(), (void *)(data.player_base + PLAYER_ACTION_STATE), 0);
			}

			for (int i = 0; i < sizeof(camera_offsets) / sizeof(DWORD); ++i) {
				WriteInt(GetProcess(), (void *)(data.camera_base + camera_offsets[i]), *(DWORD *)(state->camera + camera_offsets[i]));
			}
		}
	}
}