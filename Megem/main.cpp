#include "stdafx.h"

int dataAddress = 0;
HANDLE process = NULL;

void MainThread() {
	LoadLibraryA("DLL.dll");
	
	int pid, tpid;

	PROCESSENTRY32 entry;
	HANDLE snapshot;

	entry.dwSize = sizeof(PROCESSENTRY32);

	for (pid = tpid = 0;;) {
		snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		tpid = 0;

		if (Process32First(snapshot, &entry)) {
			do {
				if (_wcsicmp(entry.szExeFile, TEXT("MirrorsEdge.exe")) == 0) {
					tpid = entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &entry));
		}

		if (!tpid) {
			pid = dataAddress = 0;
			process = NULL;

			CheckMenuItem(GetMenu(GetHWND()), ID_PLAYER_GOD, MF_UNCHECKED);
			CheckMenuItem(GetMenu(GetHWND()), ID_PLAYER_FLOAT, MF_UNCHECKED);
		} else if (pid != tpid) {
			pid = tpid;

			CloseHandle(process);
			process = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

			char path[MAX_PATH];
			path[GetTempPathA(MAX_PATH, path)] = '\0';
			strcat(path, "megem_data");

			if (!ProcessHasModule(pid, TEXT("DLL.dll"))) {
				remove(path);

				char dllPath[MAX_PATH];
				GetFullPathNameA("DLL.dll", MAX_PATH, dllPath, NULL);

				LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
				LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				WriteProcessMemory(process, arg, dllPath, strlen(dllPath), NULL);

				HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, arg, 0, NULL);

				WaitForSingleObject(thread, INFINITE);

				VirtualFreeEx(process, arg, 0, MEM_RELEASE);

				while (!FileExists(path)) {
					Sleep(100);
				}
			}

			FILE *file = fopen(path, "rb");
			fread(&dataAddress, sizeof(int), 1, file);
			fclose(file);
		}

		CloseHandle(snapshot);

		Sleep(5000);
	}
}

HANDLE GetProcess() {
	return process;
}

HANDLE CallFunction(char *name, void *arg) {
	if (!process) {
		return NULL;
	}

	int base = ModuleBase(GetProcessId(process), TEXT("DLL.dll"));
	if (!base) {
		return NULL;
	}

	int offset = (int)GetProcAddress(GetModuleHandleA("DLL.dll"), name) - (int)GetModuleHandleA("DLL.dll");

	return CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)(base + offset), arg, 0, NULL);
}

DATA *GetData() {
	static DATA data;

	if (process && dataAddress) {
		ReadProcessMemory(process, (LPCVOID)dataAddress, &data, sizeof(DATA), NULL);
	} else {
		memset(&data, 0, sizeof(DATA));
	}

	return &data;
}

int ProcessHasModule(int pid, wchar_t *module) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szModule, module) == 0) {
				return 1;
			}
		} while (Module32Next(snapshot, &entry));
	}

	return 0;
}

int ModuleBase(int pid, wchar_t *module) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szModule, module) == 0) {
				return (int)entry.modBaseAddr;
			}
		} while (Module32Next(snapshot, &entry));
	}

	return 0;
}

void AppendItemToTree(HWND tree, char *text) {
	TCHAR buffer[0xFFF];
	MultiByteToWideChar(CP_ACP, 0, text, -1, buffer, 0xFFF);

	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.pszText = buffer;
	tvi.cchTextMax = 0xFFF;
	tvi.lParam = (LPARAM)0;

	tvins.item = tvi;

	tvins.hInsertAfter = TreeView_GetRoot(tree);
	for (int i = 0, l = TreeView_GetCount(tree); i < l; i++) {
		tvins.hInsertAfter = TreeView_GetNextItem(tree, tvins.hInsertAfter, TVGN_NEXT);
	}

	tvins.hParent = TVI_ROOT;

	SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
}

int GetTreeSelectedIndex(HWND tree) {
	HTREEITEM item = TreeView_GetRoot(tree);

	for (int i = 0, l = TreeView_GetCount(tree); i < l; i++) {
		if (TreeView_GetSelection(tree) == item) {
			return i;
		}

		item = TreeView_GetNextItem(tree, item, TVGN_NEXT);
	}

	return -1;
}

void prepend(char *dest, char *src) {
	int len = strlen(src);

	memmove(dest + len, dest, strlen(dest) + 1);

	for (int i = 0; i < len; i++) {
		dest[i] = src[i];
	}
}

void SuspendCurrentProcess() {
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	DWORD pid = GetCurrentProcessId();

	DWORD dllId = GetCurrentThreadId();

	if (Thread32First(snapshot, &entry)) {
		do {
			if (entry.th32OwnerProcessID == pid && entry.th32ThreadID != dllId) {
				HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0, entry.th32ThreadID);
				SuspendThread(thread);
				CloseHandle(thread);
			}
		} while (Thread32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
}

void ResumeCurrentProcess() {
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	DWORD pid = GetCurrentProcessId();

	if (Thread32First(snapshot, &entry)) {
		do {
			if (entry.th32OwnerProcessID == pid) {
				HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0, entry.th32ThreadID);
				ResumeThread(thread);
				CloseHandle(thread);
			}
		} while (Thread32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
}

float IntToDegrees(int i) {
	float r = (float)fmod(((float)i / 0x10000) * 360.0, 360);
	return r < 0 ? r + 360 : r;
}

int DegreesToInt(float i) {
	return (int)((i / 360) * 0x10000) % 0x10000;
}

void SaveState(STATE *state) {
	if (!GetProcess()) {
		return;
	}

	for (int i = 0, base = GetData()->player_base; i < LENGTH(STATE_PLAYER_DATA); i++) {
		ReadProcessMemory(GetProcess(), (void *)(base + STATE_PLAYER_DATA[i]), &state->player_data[i], i < 5 ? 1 : 4, NULL);
	}

	for (int i = 0, base = GetData()->camera_base; i < LENGTH(STATE_CAMERA_DATA); i++) {
		ReadProcessMemory(GetProcess(), (void *)(base + STATE_CAMERA_DATA[i]), &state->camera_data[i], 4, NULL);
	}
}

void LoadState(STATE *state) {
	if (!GetProcess()) {
		return;
	}

	for (int i = 0, base = GetData()->player_base; i < LENGTH(STATE_PLAYER_DATA); i++) {
		WriteProcessMemory(GetProcess(), (void *)(base + STATE_PLAYER_DATA[i]), &state->player_data[i], i < 5 ? 1 : 4, NULL);
	}

	for (int i = 0, base = GetData()->camera_base; i < LENGTH(STATE_CAMERA_DATA); i++) {
		WriteProcessMemory(GetProcess(), (void *)(base + STATE_CAMERA_DATA[i]), &state->camera_data[i], 4, NULL);
	}
}

int FileExists(char *path) {
	struct stat buffer;
	return stat(path, &buffer) == 0;
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

int GetProcessCount(TCHAR *process) {
	PROCESSENTRY32 entry;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	entry.dwSize = sizeof(PROCESSENTRY32);

	int c = 0;

	if (Process32First(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, process) == 0) {
				c++;
			}
		} while (Process32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);

	return c;
}