#include "stdafx.h"

HWND global_hWnd;
HINSTANCE hInst;
WNDPROC visualPropertyProc;
HHOOK keyboardHook;
struct {
	int focus, suspend, process_speed_0, process_speed_quarter, process_speed_half, process_speed_3quarters, process_speed_normal, process_speed_double, pause, engine_speed_0, engine_speed_quarter, engine_speed_half, engine_speed_3quarters, engine_speed_normal, engine_speed_double, godmode, flymode, fly_increase_speed, fly_decrease_speed, kickglitch, reactiontime, strang, save_slot_0, save_slot_1, save_slot_2, save_slot_3, save_slot_4, save_slot_5, save_slot_6, save_slot_7, save_slot_8, save_slot_9, load_slot_0, load_slot_1, load_slot_2, load_slot_3, load_slot_4, load_slot_5, load_slot_6, load_slot_7, load_slot_8, load_slot_9;
} KEYBINDS;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	if (GetProcessCount(TEXT("Megem.exe")) > 1) {
		return 0;
	}

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);

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
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MEGEM);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEGEM));

	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, 0, 0);
	SetupKeybinds();
	SetTimer(hWnd, 0, 17, NULL);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

int ParseKeybind(char *name) {
	for (int i = 0; i < LENGTH(KEYS); i++) {
		if (_stricmp(name, KEYS[i].name) == 0) {
			return KEYS[i].keycode;
		}
	}

	return -1;
}

void SetupKeybinds() {
	char focus[0xFF], suspend[0xFF], pspeed0[0xFF], pspeed25[0xFF], pspeed5[0xFF], pspeed75[0xFF], pspeed1[0xFF], pspeed2[0xFF], pause[0xFF], espeed0[0xFF], espeed25[0xFF], espeed5[0xFF], espeed75[0xFF], espeed1[0xFF], espeed2[0xFF], god[0xFF], fly[0xFF], flyincrease[0xFF], flydecrease[0xFF], kg[0xFF], rt[0xFF], strang[0xFF], ss0[0xFF], ss1[0xFF], ss2[0xFF], ss3[0xFF], ss4[0xFF], ss5[0xFF], ss6[0xFF], ss7[0xFF], ss8[0xFF], ss9[0xFF], ls0[0xFF], ls1[0xFF], ls2[0xFF], ls3[0xFF], ls4[0xFF], ls5[0xFF], ls6[0xFF], ls7[0xFF], ls8[0xFF], ls9[0xFF];

	FILE *file = fopen("keybinds.dat", "r");

	fscanf(file, "focus:%s suspend:%s pspeed0:%s pspeed.25:%s pspeed.5:%s pspeed.75:%s pspeed1:%s pspeed2:%s pause:%s espeed0:%s espeed.25:%s espeed.5:%s espeed.75:%s espeed1:%s espeed2:%s god:%s fly:%s flyincrease:%s flydecrease:%s kg:%s rt:%s strang:%s ss0:%s ss1:%s ss2:%s ss3:%s ss4:%s ss5:%s ss6:%s ss7:%s ss8:%s ss9:%s ls0:%s ls1:%s ls2:%s ls3:%s ls4:%s ls5:%s ls6:%s ls7:%s ls8:%s ls9:%s", focus, suspend, pspeed0, pspeed25, pspeed5, pspeed75, pspeed1, pspeed2, pause, espeed0, espeed25, espeed5, espeed75, espeed1, espeed2, god, fly, flyincrease, flydecrease, kg, rt, strang, ss0, ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9, ls0, ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8, ls9);

	fclose(file);

	KEYBINDS.focus = ParseKeybind(focus);
	KEYBINDS.suspend = ParseKeybind(suspend);
	KEYBINDS.process_speed_0 = ParseKeybind(pspeed0);
	KEYBINDS.process_speed_quarter = ParseKeybind(pspeed25);
	KEYBINDS.process_speed_half = ParseKeybind(pspeed5);
	KEYBINDS.process_speed_3quarters = ParseKeybind(pspeed75);
	KEYBINDS.process_speed_normal = ParseKeybind(pspeed1);
	KEYBINDS.process_speed_double = ParseKeybind(pspeed2);
	KEYBINDS.pause = ParseKeybind(pause);
	KEYBINDS.engine_speed_0 = ParseKeybind(espeed0);
	KEYBINDS.engine_speed_quarter = ParseKeybind(espeed25);
	KEYBINDS.engine_speed_half = ParseKeybind(espeed5);
	KEYBINDS.engine_speed_3quarters = ParseKeybind(espeed75);
	KEYBINDS.engine_speed_normal = ParseKeybind(espeed1);
	KEYBINDS.engine_speed_double = ParseKeybind(espeed2);
	KEYBINDS.godmode = ParseKeybind(god);
	KEYBINDS.flymode = ParseKeybind(fly);
	KEYBINDS.fly_increase_speed = ParseKeybind(flyincrease);
	KEYBINDS.fly_decrease_speed = ParseKeybind(flydecrease);
	KEYBINDS.kickglitch = ParseKeybind(kg);
	KEYBINDS.reactiontime = ParseKeybind(rt);
	KEYBINDS.strang = ParseKeybind(strang);
	KEYBINDS.save_slot_0 = ParseKeybind(ss0);
	KEYBINDS.save_slot_1 = ParseKeybind(ss1);
	KEYBINDS.save_slot_2 = ParseKeybind(ss2);
	KEYBINDS.save_slot_3 = ParseKeybind(ss3);
	KEYBINDS.save_slot_4 = ParseKeybind(ss4);
	KEYBINDS.save_slot_5 = ParseKeybind(ss5);
	KEYBINDS.save_slot_6 = ParseKeybind(ss6);
	KEYBINDS.save_slot_7 = ParseKeybind(ss7);
	KEYBINDS.save_slot_8 = ParseKeybind(ss8);
	KEYBINDS.save_slot_9 = ParseKeybind(ss9);
	KEYBINDS.load_slot_0 = ParseKeybind(ls0);
	KEYBINDS.load_slot_1 = ParseKeybind(ls1);
	KEYBINDS.load_slot_2 = ParseKeybind(ls2);
	KEYBINDS.load_slot_3 = ParseKeybind(ls3);
	KEYBINDS.load_slot_4 = ParseKeybind(ls4);
	KEYBINDS.load_slot_5 = ParseKeybind(ls5);
	KEYBINDS.load_slot_6 = ParseKeybind(ls6);
	KEYBINDS.load_slot_7 = ParseKeybind(ls7);
	KEYBINDS.load_slot_8 = ParseKeybind(ls8);
	KEYBINDS.load_slot_9 = ParseKeybind(ls9);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static char keys[0xFFF] = { 0 };
	
	if (nCode >= 0) {
		KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT *)lParam);
		int keycode = data.vkCode;

		if (wParam == WM_KEYDOWN && !keys[keycode]) {
			keys[keycode] = 1;

			if (IsGameWindow(GetForegroundWindow())) {
				if (keycode == KEYBINDS.focus) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PROCESS_FOCUS, 0);
				}
				if (keycode == KEYBINDS.suspend) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PROCESS_SUSPEND, 0);
				}
				if (keycode == KEYBINDS.process_speed_0) {
					if (GetProcess()) {
						float speed = 0.00;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.process_speed_quarter) {
					if (GetProcess()) {
						float speed = 0.25;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.process_speed_half) {
					if (GetProcess()) {
						float speed = 0.50;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.process_speed_3quarters) {
					if (GetProcess()) {
						float speed = 0.75;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.process_speed_normal) {
					if (GetProcess()) {
						float speed = 1.00;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.process_speed_double) {
					if (GetProcess()) {
						float speed = 2.00;

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
						CallFunction("EXPORT_SetSpeed", arg);
					}
				}
				if (keycode == KEYBINDS.pause) {
					SendMessage(GetHWND(), WM_COMMAND, ID_ENGINE_PAUSE, 0);
				}
				if (keycode == KEYBINDS.engine_speed_0) {
					if (GetProcess()) {
						float speed = 0.00;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.engine_speed_quarter) {
					if (GetProcess()) {
						float speed = 0.25;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.engine_speed_half) {
					if (GetProcess()) {
						float speed = 0.50;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.engine_speed_3quarters) {
					if (GetProcess()) {
						float speed = 0.75;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.engine_speed_normal) {
					if (GetProcess()) {
						float speed = 1.00;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.engine_speed_double) {
					if (GetProcess()) {
						float speed = 2.00;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
					}
				}
				if (keycode == KEYBINDS.godmode) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PLAYER_GOD, 0);
				}
				if (keycode == KEYBINDS.flymode) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PLAYER_FLOAT, 0);
				}
				if (keycode == KEYBINDS.fly_increase_speed) {
					if (GetProcess()) {
						CallFunction("EXPORT_EnableFloatIncrease", NULL);
					}
				}
				if (keycode == KEYBINDS.fly_decrease_speed) {
					if (GetProcess()) {
						CallFunction("EXPORT_EnableFloatDecrease", NULL);
					}
				}
				if (keycode == KEYBINDS.kickglitch) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PLAYER_KG, 0);
				}
				if (keycode == KEYBINDS.reactiontime) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PLAYER_RT, 0);
				}
				if (keycode == KEYBINDS.strang) {
					SendMessage(GetHWND(), WM_COMMAND, ID_PLAYER_STRANG, 0);
				}
				if (keycode == KEYBINDS.save_slot_0) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT0, 0);
				}
				if (keycode == KEYBINDS.save_slot_1) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT1, 0);
				}
				if (keycode == KEYBINDS.save_slot_2) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT2, 0);
				}
				if (keycode == KEYBINDS.save_slot_3) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT3, 0);
				}
				if (keycode == KEYBINDS.save_slot_4) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT4, 0);
				}
				if (keycode == KEYBINDS.save_slot_5) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT5, 0);
				}
				if (keycode == KEYBINDS.save_slot_6) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT6, 0);
				}
				if (keycode == KEYBINDS.save_slot_7) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT7, 0);
				}
				if (keycode == KEYBINDS.save_slot_8) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT8, 0);
				}
				if (keycode == KEYBINDS.save_slot_9) {
					SendMessage(GetHWND(), WM_COMMAND, ID_SAVESTATE_SLOT9, 0);
				}
				if (keycode == KEYBINDS.load_slot_0) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT0, 0);
				}
				if (keycode == KEYBINDS.load_slot_1) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT1, 0);
				}
				if (keycode == KEYBINDS.load_slot_2) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT2, 0);
				}
				if (keycode == KEYBINDS.load_slot_3) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT3, 0);
				}
				if (keycode == KEYBINDS.load_slot_4) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT4, 0);
				}
				if (keycode == KEYBINDS.load_slot_5) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT5, 0);
				}
				if (keycode == KEYBINDS.load_slot_6) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT6, 0);
				}
				if (keycode == KEYBINDS.load_slot_7) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT7, 0);
				}
				if (keycode == KEYBINDS.load_slot_8) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT8, 0);
				}
				if (keycode == KEYBINDS.load_slot_9) {
					SendMessage(GetHWND(), WM_COMMAND, ID_LOADSTATE_SLOT9, 0);
				}
			}
		} else if (wParam == WM_KEYUP && keys[keycode]) {
			keys[keycode] = 0;

			if (IsGameWindow(GetForegroundWindow())) {
				if (keycode == KEYBINDS.fly_increase_speed) {
					if (GetProcess()) {
						CallFunction("EXPORT_DisableFloatIncrease", NULL);
					}
				}
				if (keycode == KEYBINDS.fly_decrease_speed) {
					if (GetProcess()) {
						CallFunction("EXPORT_DisableFloatDecrease", NULL);
					}
				}
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND GetHWND() {
	return global_hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static RECT rect = { 0, 0, WIDTH, HEIGHT };
	static HDC hdc, hdcOld;
	static HBITMAP hbmMem;
	static HANDLE hOld;

	global_hWnd = hWnd;

	switch (message) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			switch (wmId) {
				case ID_FILE_KEYBINDS:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_KEYBINDS), hWnd, KeybindsProc), SW_SHOW);
					break;
				case IDM_EXIT:
					UnhookWindowsHookEx(keyboardHook);
					DestroyWindow(hWnd);
					break;
				case ID_PROCESS_FOCUS: {
					HWND hWnd = FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0);

					if (hWnd) {
						SetForegroundWindow(hWnd);
						SetActiveWindow(hWnd);
						SetFocus(hWnd);
					}

					break;
				}
				case ID_PROCESS_EXIT:
					if (GetProcess()) {
						TerminateProcess(GetProcess(), 0);
					}

					break;
				case ID_PROCESS_SUSPEND: {
					if (GetProcess()) {
						char text[0xFF];
						GetMenuStringA(GetMenu(hWnd), ID_PROCESS_SUSPEND, text, 0xFF, 0);

						bool suspend;

						if (strcmp(text, "Suspend") == 0) {
							MENUITEMINFOA item = { sizeof(item) };
							item.fMask = MIIM_TYPE | MIIM_DATA;
							GetMenuItemInfoA(GetMenu(hWnd), ID_PROCESS_SUSPEND, 0, &item);
							item.dwTypeData = "Resume";
							SetMenuItemInfoA(GetMenu(hWnd), ID_PROCESS_SUSPEND, 0, &item);

							suspend = true;
						} else {
							MENUITEMINFOA item = { sizeof(item) };
							item.fMask = MIIM_TYPE | MIIM_DATA;
							GetMenuItemInfoA(GetMenu(hWnd), ID_PROCESS_SUSPEND, 0, &item);
							item.dwTypeData = "Suspend";
							SetMenuItemInfoA(GetMenu(hWnd), ID_PROCESS_SUSPEND, 0, &item);

							suspend = false;
						}

						THREADENTRY32 entry;
						entry.dwSize = sizeof(THREADENTRY32);
						HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
						DWORD pid = GetProcessId(GetProcess());

						if (Thread32First(snapshot, &entry)) {
							do {
								if (entry.th32OwnerProcessID == pid) {
									HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0, entry.th32ThreadID);

									if (suspend) {
										SuspendThread(thread);
									} else {
										ResumeThread(thread);
									}

									CloseHandle(thread);
								}
							} while (Thread32Next(snapshot, &entry));
						}

						CloseHandle(snapshot);
					}

					break;
				}
				case ID_PROCESS_SPEED:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_PROCESS_SPEED), hWnd, ProcessSpeedProc), SW_SHOW);
					break;
				case ID_ENGINE_PAUSE: {
					if (GetProcess()) {
						char text[0xFF];
						GetMenuStringA(GetMenu(hWnd), ID_ENGINE_PAUSE, text, 0xFF, 0);

						char pause;
						if (strcmp(text, "Pause") == 0) {
							MENUITEMINFOA item = { sizeof(item) };
							item.fMask = MIIM_TYPE | MIIM_DATA;
							GetMenuItemInfoA(GetMenu(hWnd), ID_ENGINE_PAUSE, 0, &item);
							item.dwTypeData = "Resume";
							SetMenuItemInfoA(GetMenu(hWnd), ID_ENGINE_PAUSE, 0, &item);

							pause = 1;
						} else {
							MENUITEMINFOA item = { sizeof(item) };
							item.fMask = MIIM_TYPE | MIIM_DATA;
							GetMenuItemInfoA(GetMenu(hWnd), ID_ENGINE_PAUSE, 0, &item);
							item.dwTypeData = "Pause";
							SetMenuItemInfoA(GetMenu(hWnd), ID_ENGINE_PAUSE, 0, &item);

							pause = 0;
						}

						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_PAUSED), &pause, 1, NULL);
					}

					break;
				}
				case ID_ENGINE_SPEED:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_ENGINE_SPEED), hWnd, EngineSpeedProc), SW_SHOW);
					break;
				case ID_ENGINE_LEVELSTREAMER:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_LEVELSTREAMER), hWnd, LevelStreamerProc), SW_SHOW);
					break;
				case ID_VISUAL_FIELDOFVIEW:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_FOV), hWnd, FOVProc), SW_SHOW);
					break;
				case ID_VISUAL_DOLLYCAMERA:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_DOLLY_CAMERA), hWnd, DollyCameraProc), SW_SHOW);
					break;
				case ID_VISUAL_PROPERTIES:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_VISUAL_PROPERTIES), hWnd, VisualPropertiesProc), SW_SHOW);
					break;
				case ID_PLAYER_GOD:
					if (GetProcess()) {
						if (CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_UNCHECKED);

							CallFunction("EXPORT_DisableGodMode", NULL);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_GOD, MF_CHECKED);

							CallFunction("EXPORT_EnableGodMode", NULL);
						}
					}

					break;
				case ID_PLAYER_FLOAT:
					if (GetProcess()) {
						if (CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_CHECKED) == MF_CHECKED) {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_UNCHECKED);

							CallFunction("EXPORT_DisableFloatMode", NULL);
						} else {
							CheckMenuItem(GetMenu(hWnd), ID_PLAYER_FLOAT, MF_CHECKED);

							CallFunction("EXPORT_EnableFloatMode", NULL);
						}
					}

					break;
				case ID_POSITION_X:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_POSITION_X), hWnd, PositionXProc), SW_SHOW);
					break;
				case ID_POSITION_Y:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_POSITION_Y), hWnd, PositionYProc), SW_SHOW);
					break;
				case ID_POSITION_Z:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_POSITION_Z), hWnd, PositionZProc), SW_SHOW);
					break;
				case ID_VELOCITY_X:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_VELOCITY_X), hWnd, VelocityXProc), SW_SHOW);
					break;
				case ID_VELOCITY_Y:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_VELOCITY_Y), hWnd, VelocityYProc), SW_SHOW);
					break;
				case ID_VELOCITY_Z:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_VELOCITY_Z), hWnd, VelocityZProc), SW_SHOW);
					break;
				case ID_ROTATION_X:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_ROTATION_X), hWnd, RotationXProc), SW_SHOW);
					break;
				case ID_ROTATION_Y:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_ROTATION_Y), hWnd, RotationYProc), SW_SHOW);
					break;
				case ID_ROTATION_Z:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_ROTATION_Z), hWnd, RotationZProc), SW_SHOW);
					break;
				case ID_SCALE_PROPORTIONAL:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_SCALE), hWnd, ScaleProc), SW_SHOW);
					break;
				case ID_SCALE_X:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_SCALE_X), hWnd, ScaleXProc), SW_SHOW);
					break;
				case ID_SCALE_Y:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_SCALE_Y), hWnd, ScaleYProc), SW_SHOW);
					break;
				case ID_SCALE_Z:
					ShowWindow(CreateDialog(hInst, MAKEINTRESOURCE(IDD_SCALE_Z), hWnd, ScaleZProc), SW_SHOW);
					break;
				case ID_SAVESTATE_SLOT0: case ID_SAVESTATE_SLOT1: case ID_SAVESTATE_SLOT2: case ID_SAVESTATE_SLOT3: case ID_SAVESTATE_SLOT4: case ID_SAVESTATE_SLOT5: case ID_SAVESTATE_SLOT6: case ID_SAVESTATE_SLOT7: case ID_SAVESTATE_SLOT8: case ID_SAVESTATE_SLOT9: {
					if (GetProcess()) {
						int slot = wmId - ID_SAVESTATE_SLOT0;

						char path[0xFF];
						path[GetCurrentDirectoryA(0xFF, path)] = '\0';
						strcat(path, "/saves");

						CreateDirectoryA(path, NULL);

						sprintf(path + strlen(path), "/slot_%d", slot);

						FILE *file = fopen(path, "wb");
						if (file) {
							STATE state;
							SaveState(&state);

							fwrite(&state, sizeof(STATE), 1, file);
						}
						fclose(file);
					}

					break;
				}
				case ID_LOADSTATE_SLOT0: case ID_LOADSTATE_SLOT1: case ID_LOADSTATE_SLOT2: case ID_LOADSTATE_SLOT3: case ID_LOADSTATE_SLOT4: case ID_LOADSTATE_SLOT5: case ID_LOADSTATE_SLOT6: case ID_LOADSTATE_SLOT7: case ID_LOADSTATE_SLOT8: case ID_LOADSTATE_SLOT9: {
					if (GetProcess()) {
						int slot = wmId - ID_LOADSTATE_SLOT0;

						char path[0xFF];
						path[GetCurrentDirectoryA(0xFF, path)] = '\0';
						strcat(path, "/saves");

						CreateDirectoryA(path, NULL);

						sprintf(path + strlen(path), "/slot_%d", slot);

						if (FileExists(path)) {
							FILE *file = fopen(path, "rb");
							if (file) {
								STATE state;

								fread(&state, sizeof(STATE), 1, file);

								CallFunction("EXPORT_StopStrang", NULL);
								CallFunction("EXPORT_StopKickGlitch", NULL);

								LoadState(&state);
							}
							fclose(file);
						}
					}

					break;
				}
				case ID_PLAYER_RT:
					if (GetProcess()) {
						float a = 100;
						WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->camera_base + CAMERA_REACTION_TIME), &a, 4, NULL);
					}

					break;
				case ID_PLAYER_KG:
					if (GetProcess()) {
						CallFunction("EXPORT_KickGlitch", NULL);
					}

					break;
				case ID_PLAYER_STRANG:
					if (GetProcess()) {
						CallFunction("EXPORT_Strang", NULL);
					}

					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}

			break;
		}
		case WM_TIMER: {
			static DATA *data;
			static char buffer[0xFFFF];

			data = GetData();

			hdcOld = GetDC(hWnd);
			hdc = CreateCompatibleDC(hdcOld);
			hbmMem = CreateCompatibleBitmap(hdcOld, WIDTH, HEIGHT);
			hOld = SelectObject(hdc, hbmMem);
			FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

			rect.right -= 45;

			sprintf(buffer,
				/* x   */ "%f\n"\
				/* y   */ "%f\n"\
				/* z   */ "%f\n"\
				/* hv  */ "%f\n"\
				/* vv  */ "%f\n"\
				/* vx  */ "%f\n"\
				/* vy  */ "%f\n"\
				/* vz  */ "%f\n\n"\
				/* rx  */ "%f\n"\
				/* ry  */ "%f\n"\
				/* rz  */ "%f\n"\
				/* brx */ "%f\n"\
				/* bry */ "%f\n"\
				/* brz */ "%f\n\n"\
				/* h   */ "%d\n"\
				/* s   */ "%f\n",
				data->x, data->y, data->z, sqrt((data->vx * data->vx) + (data->vy * data->vy)) * 0.036, sqrt(data->vz * data->vz) * 0.036, data->vx, data->vy, data->vz, data->rx, data->ry, data->rz, data->body_rx, data->body_ry, data->body_rz, data->health, data->speed);

			DrawTextA(hdc, buffer, -1, &rect, DT_RIGHT | DT_WORDBREAK);

			rect.right += 45;

			rect.left += 25;

			strcpy(buffer, "x\ny\nz\nhv\nvv\nvx\nvy\nvz\n\nrx\nry\nrz\nbrx\nbry\nbrz\n\nh\ns");
			DrawTextA(hdc, buffer, -1, &rect, DT_LEFT | DT_WORDBREAK);

			rect.left -= 25;

			BitBlt(hdcOld, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, SRCCOPY);
			SelectObject(hdc, hOld);
			DeleteObject(hbmMem);
			DeleteDC(hdc);
			ReleaseDC(hWnd, hdcOld);

			break;
		}
		case WM_ERASEBKGND:
			return (LRESULT)1;
		case WM_DESTROY:
			UnhookWindowsHookEx(keyboardHook);
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR CALLBACK ProcessSpeedProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->process_speed);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float speed = (float)atof(text);

					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					CallFunction("EXPORT_SetSpeed", arg);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK EngineSpeedProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->speed);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float speed = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + ENGINE_SPEED), &speed, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK LevelStreamerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND list = GetDlgItem(hDlg, IDC_LIST);

	switch (message) {
		case WM_INITDIALOG: {
			SetTimer(hDlg, 0, 10000, NULL);
		}
		case WM_TIMER: {
			int top = SendMessage(list, LB_GETTOPINDEX, 0, 0);

			if (GetProcess()) {
				SendMessage(list, LB_RESETCONTENT, 0, 0);

				WaitForSingleObject(CallFunction("EXPORT_GetSublevels", NULL), INFINITE);

				int sublevel_count = GetData()->sublevel_count;

				char sublevel_name[0xFFF];
				LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 0xFFF, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

				for (int i = 0; i < sublevel_count; i++) {
					ReadProcessMemory(GetProcess(), GetData()->sublevels + (i * 0xFFF), sublevel_name, 0xFFF, NULL);

					if (strnlen(sublevel_name, 0xFFF) > 0) {
						WriteProcessMemory(GetProcess(), arg, sublevel_name, 0xFFF, NULL);

						WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);

						char status;
						ReadProcessMemory(GetProcess(), arg, &status, 1, NULL);

						if (status & 1) {
							prepend(sublevel_name, "Loaded - ");
						} else {
							prepend(sublevel_name, "Unloaded - ");
						}

						TCHAR buffer[0xFFF];
						MultiByteToWideChar(CP_ACP, 0, sublevel_name, -1, buffer, 0xFFF);
						SendMessage(list, LB_ADDSTRING, 0, (LPARAM)buffer);
					}
				}

				VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
			}

			SendMessage(list, LB_SETTOPINDEX, top, 0);

			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_LIST: {
					switch (HIWORD(wParam)) {
						case LBN_DBLCLK: {
							if (GetProcess()) {
								TCHAR buffer[0xFFF];
								char text[0xFFF];

								int item = (int)SendMessage(list, LB_GETCURSEL, 0, 0);
								SendMessage(list, LB_GETTEXT, (WPARAM)item, (LPARAM)buffer);

								wcstombs(text, buffer, 0xFFF);

								int load = !(_strnicmp(text, "Loaded", 6) == 0);
								memmove(text, strchr(text, '-') + 2, 0xFFF);

								LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 0xFFF, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
								WriteProcessMemory(GetProcess(), arg, text, 0xFFF, NULL);

								if (load) {
									WaitForSingleObject(CallFunction("EXPORT_LoadLevelStreamByString", arg), INFINITE);

									char status = 0, waits = 0;
									while (!(status & 1) && waits++ < 10) {
										WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);
										ReadProcessMemory(GetProcess(), arg, &status, 1, NULL);
										WriteProcessMemory(GetProcess(), arg, text, 1, NULL);

										Sleep(100);
									}
								} else {
									WaitForSingleObject(CallFunction("EXPORT_UnloadLevelStreamByString", arg), INFINITE);

									char status = 1, waits = 0;
									while (status & 1 && waits++ < 10) {
										WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);
										ReadProcessMemory(GetProcess(), arg, &status, 1, NULL);
										WriteProcessMemory(GetProcess(), arg, text, 1, NULL);

										Sleep(100);
									}
								}

								VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);

								LevelStreamerProc(hDlg, WM_TIMER, 0, 0);
							}
						}
					}

					break;
				}
			}

			break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK FOVProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->fov);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float speed = (float)atof(text);

					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					CallFunction("EXPORT_SetFOV", arg);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DollyCameraProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			if (GetProcess()) {
				CallFunction("EXPORT_ResetDolly", NULL);
			}

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			switch (wmId) {
				case IDC_ADD_MARKER:
					if (GetProcess()) {
						CallFunction("EXPORT_AddDollyMarker", NULL);
					}

					break;
				case IDC_PLAY: {
					if (GetProcess()) {
						char buffer[0xFFF];
						GetWindowTextA(GetDlgItem(hDlg, IDC_DURATION), buffer, 0xFFF);
						int duration = atoi(buffer);

						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &duration, 4, NULL);

						WaitForSingleObject(CallFunction("EXPORT_SetDollyDuration", arg), INFINITE);

						GetWindowTextA(GetDlgItem(hDlg, IDC_DEPTH_OF_FIELD), buffer, 0xFFF);
						float dof = (float)atof(buffer);

						arg = VirtualAllocEx(GetProcess(), NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
						WriteProcessMemory(GetProcess(), arg, &dof, 4, NULL);

						WaitForSingleObject(CallFunction("EXPORT_SetDollyDOF", arg), INFINITE);

						CallFunction("EXPORT_PlayDolly", NULL);
					}

					break;
				}
				case IDC_RESET:
					if (GetProcess()) {
						CallFunction("EXPORT_ResetDolly", NULL);
					}

					break;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK VisualPropertiesProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND child;
	HWND list = GetDlgItem(hDlg, IDC_LIST);

	switch (message) {
		case WM_INITDIALOG: {
			LVCOLUMN lvc;

			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 150;
			lvc.pszText = TEXT("Name");
			lvc.cchTextMax = 0xFF;

			ListView_InsertColumn(list, 0, &lvc);

			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 158;
			lvc.pszText = TEXT("Value");

			ListView_InsertColumn(list, 1, &lvc);

			TCHAR buffer[0xFF];

			LVITEM item;
			item.mask = LVIF_TEXT;
			item.cchTextMax = 0xFF;
			item.iItem = 0;
			item.iSubItem = 0;
			item.pszText = buffer;

			for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
				MultiByteToWideChar(CP_ACP, 0, visual_properties[i].name, -1, buffer, 0xFF);
				ListView_InsertItem(list, &item);

				if (GetProcess()) {
					char c[0xFF];

					if (visual_properties[i].type == TYPE_FLOAT) {
						float value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%f", value);
					} else {
						int value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%d", value);
					}

					MultiByteToWideChar(CP_ACP, 0, c, -1, buffer, 0xFF);
				} else {
					wcscpy(buffer, TEXT("0"));
				}

				ListView_SetItemText(list, 0, 1, buffer);
			}

			SetTimer(hDlg, 0, 10000, NULL);

			return (INT_PTR)TRUE;
		}
		case WM_TIMER: {
			TCHAR buffer[0xFF];

			for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
				if (GetProcess()) {
					char c[0xFF];

					if (visual_properties[i].type == TYPE_FLOAT) {
						float value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%f", value);
					} else {
						int value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%d", value);
					}

					MultiByteToWideChar(CP_ACP, 0, c, -1, buffer, 0xFF);
				} else {
					wcscpy(buffer, TEXT("0"));
				}

				ListView_SetItemText(list, i, 1, buffer);
			}

			break;
		}
		case WM_CLOSE:
			DestroyWindow(child);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_NOTIFY: {
			switch (((LPNMHDR)lParam)->code) {
				case NM_DBLCLK: {
					int index = ListView_GetNextItem(list, -1, LVNI_SELECTED);

					if (index >= 0) {
						DestroyWindow(child);

						char c[0xFF];

						strcpy(c, visual_properties[index].name);
						child = CreateWindowA("EDIT", c, WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 200, 65, hDlg, 0, GetModuleHandle(0), NULL);

						if (visual_properties[index].type == TYPE_FLOAT) {
							float value;
							ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[index].offset), &value, 4, NULL);
							sprintf(c, "%f", value);
						} else {
							int value;
							ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[index].offset), &value, 4, NULL);
							sprintf(c, "%d", value);
						}

						SetWindowTextA(child, c);
						visualPropertyProc = (WNDPROC)SetWindowLong(child, GWL_WNDPROC, (LONG)VisualPropertyProc);
						VisualPropertyProc(child, -1, index, index);
					}

					break;
				}
			}

			break;
		}
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			switch (wmId) {
				case IDC_SAVE: {
					TCHAR tpath[MAX_PATH] = { 0 };

					OPENFILENAME name;
					memset(&name, 0, sizeof(OPENFILENAME));
					name.lStructSize = sizeof(OPENFILENAME);
					name.hwndOwner = NULL;
					name.lpstrFilter = TEXT("*.vprops");
					name.lpstrFile = tpath;
					name.nMaxFile = MAX_PATH;
					name.Flags = OFN_OVERWRITEPROMPT;
					name.lpstrDefExt = TEXT("vprops");

					if (GetSaveFileName(&name)) {
						char path[MAX_PATH];
						wcstombs(path, tpath, MAX_PATH);

						FILE *file = fopen(path, "wb");

						int value = 0;
						for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
							if (GetProcess()) {
								ReadProcessMemory(GetProcess(), (LPCVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
							} else {
								value = 0;
							}

							fwrite(&value, sizeof(int), 1, file);
						}

						fclose(file);
					}

					break;
				}
				case IDC_LOAD: {
					TCHAR tpath[MAX_PATH] = { 0 };

					OPENFILENAME name;
					memset(&name, 0, sizeof(OPENFILENAME));
					name.lStructSize = sizeof(OPENFILENAME);
					name.hwndOwner = NULL;
					name.lpstrFilter = TEXT("*.vprops");
					name.lpstrFile = tpath;
					name.nMaxFile = MAX_PATH;
					name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					name.lpstrDefExt = TEXT("vprops");

					if (GetOpenFileName(&name)) {
						char path[MAX_PATH];
						wcstombs(path, tpath, MAX_PATH);

						FILE *file = fopen(path, "rb");

						int value = 0;
						for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
							fread(&value, sizeof(int), 1, file);

							if (GetProcess()) {
								WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + visual_properties[i].offset), &value, 4, NULL);
							}
						}

						fclose(file);
					}

					SendMessage(hDlg, WM_TIMER, 0, 0);

					break;
				}
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK VisualPropertyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int index;

	switch (message) {
		case -1:
			if (wParam == lParam) {
				index = wParam;
			}

			break;
		case WM_KEYDOWN: {
			if (wParam == VK_RETURN) {
				char c[0xFF];

				GetWindowTextA(hWnd, c, 0xFF);
				if (visual_properties[index].type == TYPE_FLOAT) {
					float value = (float)atof(c);
					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + visual_properties[index].offset), &value, 4, NULL);
				} else {
					int value = atoi(c);
					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->engine_base + visual_properties[index].offset), &value, 4, NULL);
				}

				SendMessage(GetWindow(hWnd, GW_OWNER), WM_TIMER, 0, 0);

				return DestroyWindow(hWnd);
			}

			break;
		}
	}

	return CallWindowProc(visualPropertyProc, hWnd, message, wParam, lParam);
}

INT_PTR CALLBACK PositionXProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->x);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float x = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_X), &x, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PositionYProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->y);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float y = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_Y), &y, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK PositionZProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->z);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float z = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_Z), &z, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK VelocityXProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->vx);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float vx = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_VX), &vx, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK VelocityYProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->vy);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float vy = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_VY), &vy, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK VelocityZProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->vz);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float vz = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_VZ), &vz, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK RotationXProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->rx);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					int rx = DegreesToInt((float)atof(text));

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->camera_base + CAMERA_RX), &rx, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK RotationYProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->ry);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					int ry = DegreesToInt((float)atof(text));

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->camera_base + PLAYER_RY), &ry, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK RotationZProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->rz);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					int rz = DegreesToInt((float)atof(text));

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->camera_base + PLAYER_RZ), &rz, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ScaleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->scale);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float s = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_SCALE), &s, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ScaleXProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->sx);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float sx = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_SX), &sx, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ScaleYProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->sy);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float sy = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_SY), &sy, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ScaleZProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData()->sz);

			SetDlgItemTextA(hDlg, IDC_INPUT, text);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			if (LOWORD(wParam) == IDC_CANCEL) {
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			} else if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float sz = (float)atof(text);

					WriteProcessMemory(GetProcess(), (LPVOID)(GetData()->player_base + PLAYER_SZ), &sz, 4, NULL);
				}

				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}
