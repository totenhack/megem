#include "stdafx.h"

DATA *data = (DATA *)calloc(sizeof(DATA), 1);
KEYBINDS keybinds;

void MainThread() {
	if (!GetModuleHandleA("MirrorsEdge.exe")) {
		return;
	}

	/* AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr); */

	SetupSpeed();
	SetupPlayer();
	SetupEngine();
	SetupRender();
	SetupLevel();
	SetupVisual();

	char path[0xFF] = { 0 };
	GetTempPathA(sizeof(path), path);
	strcat(path, "megem.keybinds");
	for (;;) {
		FILE *file = fopen(path, "rb");
		if (file) {
			fread(&keybinds, sizeof(keybinds), 1, file);
			fclose(file);
		}
		Sleep(1000);
	}
}

KEYBINDS *GetKeybinds() {
	return &keybinds;
}

DATA *GetData() {
	return data;
}

int wcsempty(const wchar_t *s) {
	while (*s != L'\0') {
		if (!iswblank(*s)) {
			return 0;
		}
		s++;
	}
	return 1;
}

int IsGameWindow(HWND hWnd) {
	char title[255];
	char check[] = "Mirror's Edge";
	GetWindowTextA(hWnd, title, 255);

	int i;
	for (i = 0; i < 13; i++) {
		if (check[i] != title[i]) {
			return 0;
		}
	}

	return 1;
}

void Keydown(SHORT k) {
	INPUT input;
	input.type = 1;
	input.ki.time = 0;
	input.ki.dwFlags = 0;
	input.ki.wScan = 0;
	input.ki.wVk = k;

	SendInput(1, &input, sizeof(input));
}

void Keyup(SHORT k) {
	INPUT input;
	input.type = 1;
	input.ki.time = 0;
	input.ki.wScan = 0;
	input.ki.wVk = k;
	input.ki.dwFlags = 0x0002;

	SendInput(1, &input, sizeof(input));
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
	}

	return TRUE;
}

