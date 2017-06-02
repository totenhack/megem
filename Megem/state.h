#pragma once

typedef struct {
	char player[0x730];
	char camera[0x730];
	char level[0xFF];
} STATE;

STATE SaveState();
void LoadState(STATE *state);