#include "stdafx.h"

int player_base, camera_base;

int(__thiscall *UpdatePlayerOriginal)(int, int, int);
void(*UpdateMenuOriginal)();
void(*UpdateCameraOriginal)();

// Called when player is in-game
int __fastcall UpdatePlayerHook(int this_, void *idle_, int a1, int a2) {
	player_base = this_;

	return UpdatePlayerOriginal(this_, a1, a2);
}

// Called when player is at main menu
__declspec(naked) void UpdateMenuHook() {
	__asm mov player_base, ebx
		
	__asm jmp UpdateMenuOriginal
}

__declspec(naked) void UpdateCameraHook() {
	__asm {
		mov camera_base, ebp
		jmp UpdateCameraOriginal
	}
}

int GetPlayerBase() {
	return player_base;
}

int GetCameraBase() {
	return camera_base;
}

float IntToDegrees(int i) {
	float r = (float)fmod(((float)i / 0x10000) * 360.0, 360);
	return r < 0 ? r + 360 : r;
}

int DegreesToInt(float i) {
	return (int)((i / 360) * 0x10000) % 0x10000;
}

void SetupPlayer() {
	int addr;

	// addr = 0x12B5690;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\x55\x8B\xEC\x83\xE4\xF0\x81\xEC\x24\x01\x00\x00\x53\x8B\xD9\x8B\x8B\x64\x03\x00\x00\x85\xC9\x56\x57", "xxxxxxxxxxxxxxxxxxxxxxxxx");
	UpdatePlayerOriginal = (int(__thiscall *)(int, int, int))NewGate(1);
	StaticTrampolineHook((void *)addr, UpdatePlayerHook, UpdatePlayerOriginal, 1);

	// addr = 0xE41667;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\xF3\x0F\x5C\xB3\xE8\x00\x00\x00\x0F\x28\xC5\xF3\x0F\x59\xC5\x0F\x28\xCC\xF3\x0F\x59\xCC\xF3\x0F\x58\xC1\x0F\x28\xCE\xF3\x0F\x59\xCE\xF3\x0F\x58\xC1", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	UpdateMenuOriginal = (void(*)())NewGate(3);
	StaticTrampolineHook((void *)addr, UpdateMenuHook, UpdateMenuOriginal, 3);

	// addr = 0xB5C050;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\x8B\x45\x00\x8B\x90\xC4\x02\x00\x00\x8B\xCD\xFF\xD2\x85\xC0", "xxxxxxxxxxxxxxx");
	UpdateCameraOriginal = (void(*)())NewGate(4);
	StaticTrampolineHook((void *)addr, UpdateCameraHook, UpdateCameraOriginal, 4);
}