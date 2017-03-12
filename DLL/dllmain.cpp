#include "stdafx.h"

DATA *data;

void MainThread() {
	data = (DATA *)malloc(sizeof(DATA));
	memset(data, 0, sizeof(DATA));

	char path[0xFFF];
	path[GetTempPathA(0xFFF, path)] = '\0';
	strcat(path, "megem_data");

	FILE *file = fopen(path, "wb");
	fwrite(&data, sizeof(int), 1, file);
	fclose(file);

	SetupPlayer();
	SetupSpeed();
	SetupRender();
	SetupDolly();
	SetupEngine();
	SetupLevel();
}

DATA *GetData() {
	return data;
}

int GetInt(void *address) {
	static int i;
	ReadProcessMemory(GetCurrentProcess(), address, &i, sizeof(int), NULL);
	return i;
}

float GetFloat(void *address) {
	static float i;
	ReadProcessMemory(GetCurrentProcess(), address, &i, sizeof(float), NULL);
	return i;
}

char GetByte(void *address) {
	static char i;
	ReadProcessMemory(GetCurrentProcess(), address, &i, sizeof(char), NULL);
	return i;
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

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH && (DWORD)GetModuleHandleA("MirrorsEdge.exe") > 0) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
	}

	return TRUE;
}
