#include "stdafx.h"

HWND window = 0;
HANDLE process = 0;
DWORD data_base = 0;
HHOOK keyboard_hook;
bool hasCheckpoint = false;

KEYBINDS keybinds;
SETTINGS settings;
HBRUSH brush_background;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	CreateMutexA(0, FALSE, "Local\\Megem.exe");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return -1;
	}

	/* AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr); */

	char path[0xFF] = { 0 };
	GetTempPathA(sizeof(path), path);
	strcat(path, "megem.keybinds");
	FILE *file = fopen(path, "rb");
	if (!file) {
		file = fopen(path, "wb");
		memset(&keybinds, 0, sizeof(keybinds));
		keybinds.god = VkKeyScanExA('1', 0);
		keybinds.fly = VkKeyScanExA('2', 0);
		keybinds.fly_increase = VkKeyScanExA('e', 0);
		keybinds.fly_decrease = VkKeyScanExA('q', 0);
		keybinds.fly_forward = VkKeyScanExA('w', 0);
		keybinds.fly_backward = VkKeyScanExA('s', 0);
		keybinds.fly_left = VkKeyScanExA('a', 0);
		keybinds.fly_right = VkKeyScanExA('d', 0);
		keybinds.checkpoint = VkKeyScanExA('3', 0);
		keybinds.quickturn = VkKeyScanExA('q', 0);
		keybinds.s0 = VkKeyScanExA('4', 0);
		keybinds.l0 = VkKeyScanExA('5', 0);
		fwrite(&keybinds, sizeof(keybinds), 1, file);
		fclose(file);
	} else {
		fread(&keybinds, sizeof(keybinds), 1, file);
		fclose(file);
	}

	GetTempPathA(sizeof(path), path);
	strcat(path, "megem.settings");
	file = fopen(path, "rb");
	if (!file) {
		file = fopen(path, "wb");
		memset(&settings, 0, sizeof(settings));
		settings.color_active = RGB(255, 255, 255);
		settings.color_background = RGB(32, 32, 32);
		settings.color_text = RGB(0, 151, 244);
		settings.fov = 90;
		fwrite(&settings, sizeof(settings), 1, file);
		fclose(file);
	} else {
		fread(&settings, sizeof(settings), 1, file);
		fclose(file);
	}

	brush_background = CreateSolidBrush(settings.color_background);

	WCHAR szTitle[0xFF];
	WCHAR szWindowClass[0xFF];
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, sizeof(szTitle));
	LoadStringW(hInstance, IDC_MEGEM, szWindowClass, sizeof(szWindowClass));

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEGEM));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = brush_background;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MEGEM);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER, rect.right - WIDTH, rect.bottom - HEIGHT, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		return FALSE;
	}

	window = hWnd;
	SetWindowLong(hWnd, GWL_STYLE, 0);
	SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetTimer(hWnd, 0, 17, NULL);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ProcessListener, 0, 0, 0);
	keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, 0, 0);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEGEM));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

HWND GetCurrentWindow() {
	return window;
}

SETTINGS *GetSettings() {
	return &settings;
}

KEYBINDS *GetKeybinds() {
	return &keybinds;
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

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static char keys[0xFFF] = { 0 };
	static DWORD last_c = 0;
	
	if (nCode >= 0) {
		KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT *)lParam);
		int keycode = data.vkCode;

		if (wParam == WM_KEYDOWN && keycode == keybinds.checkpoint && last_c && timeGetTime() > last_c) {
			hasCheckpoint = false;
			last_c = 0;
		}

		if (wParam == WM_KEYDOWN && !keys[keycode]) {
			keys[keycode] = 1;

			if (IsGameWindow(GetForegroundWindow()) && GetProcess()) {
				if (keycode == keybinds.pause) {
					SendMessage(window, WM_COMMAND, ID_ENGINE_PAUSED, 0);
				}
				if (keycode == keybinds.speed0) {
					float speed = 0;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.speed25) {
					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), 0.25);
					float speed = 1;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.speed5) {
					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), 0.5);
					float speed = 1;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.speed75) {
					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), 0.75);
					float speed = 1;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.speed1) {
					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), 1);
					float speed = 1;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.speed2) {
					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), 2);
					float speed = 1;
					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
				if (keycode == keybinds.god) {
					SendMessage(window, WM_COMMAND, ID_PLAYER_GOD, 0);
				}
				if (keycode == keybinds.fly) {
					SendMessage(window, WM_COMMAND, ID_PLAYER_FLOAT, 0);
				}
				if (keycode == keybinds.kickglitch) {
					SendMessage(window, WM_COMMAND, ID_TOOLS_KG, 0);
				}
				if (keycode == keybinds.reactiontime) {
					SendMessage(window, WM_COMMAND, ID_TOOLS_RESETRT, 0);
				}
				if (keycode == keybinds.strang) {
					SendMessage(window, WM_COMMAND, ID_TOOLS_STRANG, 0);
				}
				if (keycode == keybinds.beamer) {
					SendMessage(window, WM_COMMAND, ID_TOOLS_BEAMER, 0);
				}
				if (keycode == keybinds.checkpoint) {
					last_c = timeGetTime() + 1000;
					SendMessage(window, WM_COMMAND, ID_PLAYER_CHECKPOINT, 0);
				}
				if (keycode == keybinds.s0) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT0, 0);
				}
				if (keycode == keybinds.s1) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT1, 0);
				}
				if (keycode == keybinds.s2) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT2, 0);
				}
				if (keycode == keybinds.s3) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT3, 0);
				}
				if (keycode == keybinds.s4) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT4, 0);
				}
				if (keycode == keybinds.s5) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT5, 0);
				}
				if (keycode == keybinds.s6) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT6, 0);
				}
				if (keycode == keybinds.s7) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT7, 0);
				}
				if (keycode == keybinds.s8) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT8, 0);
				}
				if (keycode == keybinds.s9) {
					SendMessage(window, WM_COMMAND, ID_SAVE_SLOT9, 0);
				}
				if (keycode == keybinds.l0) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT0, 0);
				}
				if (keycode == keybinds.l1) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT1, 0);
				}
				if (keycode == keybinds.l2) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT2, 0);
				}
				if (keycode == keybinds.l3) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT3, 0);
				}
				if (keycode == keybinds.l4) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT4, 0);
				}
				if (keycode == keybinds.l5) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT5, 0);
				}
				if (keycode == keybinds.l6) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT6, 0);
				}
				if (keycode == keybinds.l7) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT7, 0);
				}
				if (keycode == keybinds.l8) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT8, 0);
				}
				if (keycode == keybinds.l9) {
					SendMessage(window, WM_COMMAND, ID_LOAD_SLOT9, 0);
				}

			}
		} else if (wParam == WM_KEYUP && keys[keycode]) {
			keys[keycode] = 0;
			if (keycode == keybinds.checkpoint) {
				last_c = 0;
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DATA GetData() {
	if (process) {
		DATA d;
		ReadProcessMemory(process, (void *)data_base, &d, sizeof(DATA), NULL);
		return d;
	}

	return{ 0 };
}

HANDLE GetProcess() {
	return process;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int drag_x = 0, drag_y = 0;
	static bool uncap = false, god = false, fly = false;
	static float cx = 0, cy = 0, cz = 0, cf = 0;
	static DWORD cs = 0;

	switch (message) {
		case WM_CREATE: {
			MENUINFO mi = { 0 };
			mi.cbSize = sizeof(mi);
			mi.fMask = MIM_BACKGROUND;
			mi.hbrBack = CreateSolidBrush(settings.color_text);
			HMENU menu = GetMenu(hWnd);
			SetMenuItemBitmaps(menu, 0, MF_BYPOSITION, NULL, 0);
			SetMenuInfo(menu, &mi);
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_FILE_KEYBINDS:
					CreatePrompt(IDD_KEYBINDS, KeybindsProc);
					break;
				case ID_FILE_SETTINGS:
					CreatePrompt(IDD_SETTINGS, SettingsProc);
					break;
				case 0: case ID_FILE_EXIT:
					UnhookWindowsHookEx(keyboard_hook);
					if (process) {
						ResumeProcess(GetProcessId(process));
						CallFunction("EXPORT_GodOff", 0);
						CallFunction("EXPORT_FloatOff", 0);
						CallFunction("EXPORT_CloseDebugConsole", 0);
						CallFunction("EXPORT_DisableAll", 0);
						float speed = 1;
						LPVOID arg = VirtualAllocEx(process, NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(process, arg, &speed, sizeof(float), NULL);
						WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
						VirtualFreeEx(process, arg, 0, MEM_RELEASE);
						WriteFloat(process, (void *)(GetData().engine_base + ENGINE_SPEED), 1);
						WriteFloat(process, (void *)(GetData().uncap_base + 0x210), 3500.0);
					}
					exit(0);
					break;
				case ID_PROCESS_FOCUS: {
					HWND hWnd = GetGameWindow();
					if (hWnd) {
						SetForegroundWindow(hWnd);
						SetActiveWindow(hWnd);
						SetFocus(hWnd);
					}

					break;
				}
				case ID_PROCESS_EXIT:
					if (process) {
						TerminateProcess(process, 0);
					}
					break;
				case ID_PROCESS_SUSPEND: {
					if (process) {
						THREADENTRY32 entry;
						entry.dwSize = sizeof(THREADENTRY32);
						HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
						DWORD pid = GetProcessId(process);

						if (Thread32First(snapshot, &entry)) {
							do {
								if (entry.th32OwnerProcessID == pid) {
									HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0, entry.th32ThreadID);
									SuspendThread(thread);
									CloseHandle(thread);
								}
							} while (Thread32Next(snapshot, &entry));
						}

						CloseHandle(snapshot);
					}
					break;
				}
				case ID_PROCESS_RESUME: {
					if (process) {
						THREADENTRY32 entry;
						entry.dwSize = sizeof(THREADENTRY32);
						HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
						DWORD pid = GetProcessId(process);

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
					break;
				}
				case ID_PROCESS_SPEED:
					CreatePrompt(IDD_PROCESS_SPEED, ProcessSpeedProc);
					break;
				case ID_ENGINE_PAUSED:
					if (process) {
						if (CheckMenuItem(GetMenu(hWnd), ID_ENGINE_PAUSED, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_ENGINE_PAUSED, MF_UNCHECKED);
							WriteInt(process, (void *)(GetData().engine_base + ENGINE_PAUSED), 0);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_ENGINE_PAUSED, MF_CHECKED);
							WriteInt(process, (void *)(GetData().engine_base + ENGINE_PAUSED), 16843009);
						}
					}
					break;
				case ID_ENGINE_SPEED:
					CreatePrompt(IDD_ENGINE_SPEED, EngineSpeedProc);
					break;
				case ID_ENGINE_DEBUGCONSOLE:
					if (process) {
						if (CheckMenuItem(GetMenu(hWnd), ID_ENGINE_DEBUGCONSOLE, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_ENGINE_DEBUGCONSOLE, MF_UNCHECKED);
							CallFunction("EXPORT_CloseDebugConsole", 0);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_ENGINE_DEBUGCONSOLE, MF_CHECKED);
							CallFunction("EXPORT_OpenDebugConsole", 0);
						}
					}
					break;
				case ID_ENGINE_LEVELSTREAMER:
					CreatePrompt(IDD_LEVELSTREAMER, LevelStreamerProc);
					break;
				case ID_VISUAL_FOV:
					CreatePrompt(IDD_FOV, FOVProc);
					break;
				case ID_VISUAL_DOLLYCAMERA:
					CreatePrompt(IDD_DOLLYCAMERA, DollyCameraProc);
					break;
				case ID_VISUAL_PROPERTIES:
					CreatePrompt(IDD_VISUAL_PROPERTIES, VisualPropertiesProc);
					break;
				case ID_POSITION_X:
					CreatePrompt(IDD_PLAYER_X, PlayerXProc);
					break;
				case ID_POSITION_Y:
					CreatePrompt(IDD_PLAYER_Y, PlayerYProc);
					break;
				case ID_POSITION_Z:
					CreatePrompt(IDD_PLAYER_Z, PlayerZProc);
					break;
				case ID_VELOCITY_X:
					CreatePrompt(IDD_PLAYER_VX, PlayerVXProc);
					break;
				case ID_VELOCITY_Y:
					CreatePrompt(IDD_PLAYER_VY, PlayerVYProc);
					break;
				case ID_VELOCITY_Z:
					CreatePrompt(IDD_PLAYER_VZ, PlayerVZProc);
					break;
				case ID_ROTATION_X:
					CreatePrompt(IDD_PLAYER_RX, PlayerRXProc);
					break;
				case ID_ROTATION_Y:
					CreatePrompt(IDD_PLAYER_RY, PlayerRYProc);
					break;
				case ID_ROTATION_Z:
					CreatePrompt(IDD_PLAYER_RZ, PlayerRZProc);
					break;
				case ID_SCALE_PROPORTIONAL:
					CreatePrompt(IDD_PLAYER_SCALE, PlayerScaleProc);
					break;
				case ID_SCALE_X:
					CreatePrompt(IDD_PLAYER_SX, PlayerSXProc);
					break;
				case ID_SCALE_Y:
					CreatePrompt(IDD_PLAYER_SY, PlayerSYProc);
					break;
				case ID_SCALE_Z:
					CreatePrompt(IDD_PLAYER_SZ, PlayerSZProc);
					break;
				case ID_PLAYER_GOD:
					if (process) {
						if (CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_UNCHECKED);
							god = false;
							CallFunction("EXPORT_GodOff", 0);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_CHECKED);
							god = true;
							CallFunction("EXPORT_GodOn", 0);
						}
					}
					break;
				case ID_PLAYER_FLOAT:
					if (process) {
						if (CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_UNCHECKED);
							fly = false;
							CallFunction("EXPORT_DisableAll", 0);
							CallFunction("EXPORT_FloatOff", 0);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_CHECKED);
							fly = true;
							CallFunction("EXPORT_FloatOn", 0);
						}
					}
					break;
				case ID_PLAYER_CHECKPOINT:
					if (process) {
						hasCheckpoint = true;
						cs = 0;
						cf = 0;
						cx = ReadFloat(process, (void *)(GetData().player_base + PLAYER_X));
						cy = ReadFloat(process, (void *)(GetData().player_base + PLAYER_Y));
						cz = ReadFloat(process, (void *)(GetData().player_base + PLAYER_Z));
					}
					break;
				case ID_PLAYER_UNCAPSPEED:
					if (process) {
						if (CheckMenuItem(GetMenu(hWnd), ID_PLAYER_UNCAPSPEED, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_UNCAPSPEED, MF_UNCHECKED);
							uncap = false;
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_UNCAPSPEED, MF_CHECKED);
							uncap = true;
						}
					}
					break;
				case ID_SAVE_SLOT0:case ID_SAVE_SLOT1:case ID_SAVE_SLOT2:case ID_SAVE_SLOT3:case ID_SAVE_SLOT4:case ID_SAVE_SLOT5:case ID_SAVE_SLOT6:case ID_SAVE_SLOT7:case ID_SAVE_SLOT8:case ID_SAVE_SLOT9: {
					if (GetProcess()) {
						DWORD slot = LOWORD(wParam) - ID_SAVE_SLOT0;
						char path[0xFF];
						sprintf(path, "saves/slot_%d.medat", slot);
						FILE *file = fopen(path, "wb");
						if (file) {
							STATE state = SaveState();
							fwrite(&state, sizeof(state), 1, file);
							fclose(file);
						}
					}
					break;
				}
				case ID_LOAD_SLOT0:case ID_LOAD_SLOT1:case ID_LOAD_SLOT2:case ID_LOAD_SLOT3:case ID_LOAD_SLOT4:case ID_LOAD_SLOT5:case ID_LOAD_SLOT6:case ID_LOAD_SLOT7:case ID_LOAD_SLOT8:case ID_LOAD_SLOT9: {
					if (GetProcess()) {
						DWORD slot = LOWORD(wParam) - ID_LOAD_SLOT0;
						char path[0xFF];
						sprintf(path, "saves/slot_%d.medat", slot);
						FILE *file = fopen(path, "rb");
						if (file) {
							STATE state;
							fread(&state, sizeof(state), 1, file);
							CallFunction("EXPORT_DisableAll", 0);
							LoadState(&state);
							fclose(file);
							if (hasCheckpoint) {
								cs = timeGetTime();
								cf = 0;
							}
						}
					}
					break;
				}
				case ID_TOOLS_KG:
					CallFunction("EXPORT_KG", 0);
					break;
				case ID_TOOLS_RESETRT:
					if (process) {
						WriteFloat(process, (void *)(GetData().camera_base + CAMERA_REACTION_TIME), 100);
					}
					break;
				case ID_TOOLS_STRANG:
					CallFunction("EXPORT_Strang", 0);
					break;
				case ID_TOOLS_BEAMER:
					CallFunction("EXPORT_Beamer", 0);
					break;
			}
			break;
		}
		case WM_TIMER: {
			static char buffer[0xFFF];
			static char player[0x730];
			static DWORD camera[4];
			static float engine_speed;

			HDC hdcOld = GetDC(hWnd);
			HDC hdc = CreateCompatibleDC(hdcOld);
			HBITMAP hbmMem = CreateCompatibleBitmap(hdcOld, WIDTH, HEIGHT);
			HGDIOBJ hOld = SelectObject(hdc, hbmMem);
			RECT rect = { 0, 0, WIDTH, HEIGHT };
			FillRect(hdc, &rect, brush_background);
			SetBkColor(hdc, settings.color_background);
			SetTextColor(hdc, settings.color_text);
			HFONT font = CreateFontA(22, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
			SelectObject(hdc, font);

			float high = 0;

			if (process) {
				DATA data = GetData();
				ReadProcessMemory(process, (void *)data.player_base, player, sizeof(player), NULL);
				ReadProcessMemory(process, (void *)(data.camera_base + CAMERA_RX), camera, sizeof(DWORD) * 3, NULL);
				ReadProcessMemory(process, (void *)(data.camera_base + CAMERA_REACTION_TIME), &camera[3], sizeof(DWORD), NULL);
				ReadProcessMemory(process, (void *)(data.engine_base + ENGINE_SPEED), &engine_speed, sizeof(float), NULL);

				if (!data.loading) {
					high = data.top_speed;
					if (settings.toggle_fov) {
						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &settings.fov, sizeof(float), NULL);
						WaitForSingleObject(CallFunction("EXPORT_SetFOV", arg), INFINITE);
						VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
					}

					if (settings.toggle_vp) {
						for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
							DWORD base = i < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
							DWORD value = settings.visual_properties[i];
							WriteProcessMemory(process, (LPVOID)(base + visual_properties[i].offset), &value, 4, NULL);
						}
					}

					if (uncap) {
						WriteFloat(process, (void *)(data.uncap_base + 0x210), 2147483648.0);
					} else {
						WriteFloat(process, (void *)(data.uncap_base + 0x210), 3500.0);
					}
				}

				float dx = cx - *(float *)(player + PLAYER_X);
				float dy = cy - *(float *)(player + PLAYER_Y);
				float dz = cz - *(float *)(player + PLAYER_Z);
				if (hasCheckpoint && cs && !cf && sqrt(dx * dx + dy * dy + dz * dz) <= 250) {
					cf = (float)(timeGetTime() - cs) / 1000;
				}
			} else {
				memset(player, 0, sizeof(player));
				memset(camera, 0, sizeof(camera));
				engine_speed = 0;
			}

			float vx = *(float *)&player[PLAYER_VX];
			float vy = *(float *)&player[PLAYER_VY];

			sprintf(buffer,
				/* x */	"%.2f\n"\
				/* y */	"%.2f\n"\
				/* z */	"%.2f\n\n"\
				/* hv */"%.2f\n"\
				/* hhv*/"%.2f\n"\
				/* vv */"%.2f\n\n"\
				/* rx */"%.2f\n"\
				/* ry */"%.2f\n\n"\
				/* t  */"%.2f",
				*(float *)&player[PLAYER_X], *(float *)&player[PLAYER_Y], *(float *)&player[PLAYER_Z],
				sqrt(vx * vx + vy * vy) * 0.036, high * 0.036, fabs(*(float *)&player[PLAYER_VZ]) * 0.036,
				IntToDegrees(camera[0]), IntToDegrees(camera[1]), !cs || !hasCheckpoint ? 0.0 : cf ? cf : (float)(timeGetTime() - cs) / 1000);

			rect.top += 10;
			rect.right -= 20;
			DrawTextA(hdc, buffer, -1, &rect, DT_RIGHT | DT_WORDBREAK);

			rect.left += 20;
			strcpy(buffer, "x\ny\nz\n\nhv\nhhv\nvv\n\nrx\nry\n\nt");
			DrawTextA(hdc, buffer, -1, &rect, DT_LEFT | DT_WORDBREAK);

			rect.top += HEIGHT - 70;
			rect.left += 40;
			rect.right -= 40;

			if (god) SetTextColor(hdc, settings.color_active);
			DrawTextA(hdc, "god", -1, &rect, DT_LEFT);
			SetTextColor(hdc, settings.color_text);
			if (fly) SetTextColor(hdc, settings.color_active);
			DrawTextA(hdc, "fly", -1, &rect, DT_RIGHT);

			BitBlt(hdcOld, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, SRCCOPY);
			SelectObject(hdc, hOld);
			DeleteObject(hbmMem);
			DeleteDC(hdc);
			DeleteObject(font);
			ReleaseDC(hWnd, hdcOld);

			break;
		}
		case WM_RBUTTONDOWN: {
			HMENU menu = CreatePopupMenu();
			InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, 0, L"Exit");
			SetForegroundWindow(hWnd);

			POINT p;
			GetCursorPos(&p);

			TrackPopupMenu(menu, TPM_TOPALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hWnd, NULL);

			break;
		}
		case WM_LBUTTONDOWN: {
			SetCapture(hWnd);

			POINT p;
			GetCursorPos(&p);
			RECT r;
			GetWindowRect(hWnd, &r);

			drag_x = p.x - r.left;
			drag_y = p.y - r.top;

			break;
		}
		case WM_LBUTTONUP:
			ReleaseCapture();
			break;
		case WM_MOUSEMOVE: {
			if (wParam == MK_LBUTTON) {
				POINT p;
				GetCursorPos(&p);

				MoveWindow(hWnd, p.x - drag_x, p.y - drag_y, WIDTH, HEIGHT, true);
			}

			break;
		}
		case WM_DESTROY:
			exit(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ProcessListener() {
	LoadLibraryA("DLL.dll");

	DWORD pid, tpid;
	PROCESSENTRY32 entry;
	HANDLE snapshot;

	entry.dwSize = sizeof(entry);

	for (pid = tpid = 0;; tpid = 0) {
		snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry)) {
			do {
				if (_wcsicmp(entry.szExeFile, L"MirrorsEdge.exe") == 0) {
					tpid = entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &entry));
		}

		if (!tpid) {
			process = 0;
			CheckMenuItem(GetMenu(window), ID_ENGINE_PAUSED, MF_UNCHECKED);
			CheckMenuItem(GetMenu(window), ID_ENGINE_DEBUGCONSOLE, MF_UNCHECKED);
			CheckMenuItem(GetMenu(window), ID_PLAYER_GOD, MF_UNCHECKED);
			CheckMenuItem(GetMenu(window), ID_PLAYER_FLOAT, MF_UNCHECKED);
			CheckMenuItem(GetMenu(window), ID_PLAYER_UNCAPSPEED, MF_UNCHECKED);
		} else if (pid != tpid) {
			pid = tpid;
			if (process) {
				CloseHandle(process);
			}
			process = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

			if (process) {
				if (!GetModuleInfoByName(pid, L"DLL.dll").dwSize) {
					char path[MAX_PATH];
					GetFullPathNameA("DLL.dll", MAX_PATH, path, NULL);

					LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
					LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(path) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(process, arg, path, strlen(path) + 1, NULL);

					WaitForSingleObject(CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, arg, 0, 0), INFINITE);

					VirtualFreeEx(process, arg, 0, MEM_RELEASE);
				}

				LPVOID base = (LPVOID)VirtualAllocEx(process, NULL, sizeof(DWORD), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				WaitForSingleObject(CallFunction("EXPORT_GetDataBase", base), INFINITE);
				ReadProcessMemory(process, base, &data_base, sizeof(DWORD), NULL);
				VirtualFreeEx(process, base, 0, MEM_RELEASE);
			}
		}

		CloseHandle(snapshot);
		Sleep(2000);
	}
}

HANDLE CallFunction(char *name, void *arg) {
	if (!process) {
		return NULL;
	}

	int base = (int)GetModuleInfoByName(GetProcessId(process), TEXT("DLL.dll")).modBaseAddr;
	if (!base) {
		return NULL;
	}

	int offset = (int)GetProcAddress(GetModuleHandleA("DLL.dll"), name) - (int)GetModuleHandleA("DLL.dll");

	return CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)(base + offset), arg, 0, NULL);
}

float IntToDegrees(int i) {
	float r = (float)fmod(((float)i / 0x10000) * 360.0, 360);
	return r < 0 ? r + 360 : r;
}

int DegreesToInt(float i) {
	return (int)((i / 360) * 0x10000) % 0x10000;
}

void prepend(char *dest, char *src) {
	int len = strlen(src);

	memmove(dest + len, dest, strlen(dest) + 1);

	for (int i = 0; i < len; i++) {
		dest[i] = src[i];
	}
}
