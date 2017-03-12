#include "stdafx.h"

void SaveState(STATE *state) {
	for (int i = 0, base = GetPlayerBase(); i < LENGTH(STATE_PLAYER_DATA); i++) {
		state->player_data[i] = *(int *)(base + STATE_PLAYER_DATA[i]);
	}

	for (int i = 0, base = GetCameraBase(); i < LENGTH(STATE_CAMERA_DATA); i++) {
		state->camera_data[i] = *(int *)(base + STATE_CAMERA_DATA[i]);
	}
}

void LoadState(STATE *state) {
	for (int i = 0, base = GetPlayerBase(); i < LENGTH(STATE_PLAYER_DATA); i++) {
		*(int *)(base + STATE_PLAYER_DATA[i]) = state->player_data[i];
	}

	for (int i = 0, base = GetCameraBase(); i < LENGTH(STATE_CAMERA_DATA); i++) {
		*(int *)(base + STATE_CAMERA_DATA[i]) = state->camera_data[i];
	}
}