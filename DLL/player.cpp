#include "stdafx.h"

DWORD player_base = 0, camera_base = 0, uncap_base = 0;

int(__thiscall *UpdatePlayerOriginal)(int, int, int);
void(*UpdateCameraOriginal)();
void(*UpdateMenuOriginal)();
void(*UpdateCapOriginal)();
DWORD static_base = 0;

int __fastcall UpdatePlayerHook(int this_, void *idle_, int a1, int a2) {
	GetData()->player_base = player_base = this_;

	return UpdatePlayerOriginal(this_, a1, a2);
}

__declspec(naked) void UpdateMenuHook() {
	__asm {
		mov player_base, ebx
		push ecx
		push eax
	}

	GetData()->player_base = player_base;

	__asm {
		pop eax
		pop ecx
		jmp UpdateMenuOriginal
	}
}

__declspec(naked) void UpdateCameraHook() {
	__asm {
		mov camera_base, ebp
		push ecx
		push eax
	}

	GetData()->camera_base = camera_base;

	__asm {
		pop eax
		pop ecx
		jmp UpdateCameraOriginal
	}
}

DWORD GetPlayerBase() {
	return player_base;
	// return strcmp(GetData()->level, "TdMainMenu") == 0 ? player_base : (GetData()->player_base = (DWORD)GetPointer(GetCurrentProcess(), 5, static_base, 0xCC, 0x4A4, 0x214, 0x00));
}

DWORD GetCameraBase() {
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
	static_base = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\x89\x0D\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xFF", "xx????x????x");
	static_base = *(DWORD *)(static_base + 0x2);

	DWORD addr;

	addr = 0x12B5690;
	TrampolineHook(UpdatePlayerHook, (void *)addr, (void **)&UpdatePlayerOriginal);

	addr = 0xE41667;
	TrampolineHook(UpdateMenuHook, (void *)addr, (void **)&UpdateMenuOriginal);

	addr = 0xB5C050;
	TrampolineHook(UpdateCameraHook, (void *)addr, (void **)&UpdateCameraOriginal);
}