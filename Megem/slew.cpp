#include "stdafx.h"

HWND slew_window = 0;
bool stdout_hooked = false;

void EditAppendText(HWND edit, char *_text) {
	int length = 0;
	for (char *c = _text; *c; ++c) {
		++length;
		if (*c == '\n') ++length;
	}

	char *text = (char *)malloc(length + 1);
	*(text + length) = 0;
	for (char *c = _text, *n = text; *c; ++c, ++n) {
		if (*c == '\n') {
			*n = '\r';
			*++n = '\n';
		} else {
			*n = *c;
		}
	}

	length += GetWindowTextLengthA(edit) + 1;

	char *buffer = (char *)calloc(length, 1);
	GetWindowTextA(edit, buffer, length);

	strcat(buffer, text);

	SetWindowTextA(edit, buffer);

	free(text);
	free(buffer);
}

PromptProc(SlewProc) {
	static HANDLE thread = 0;

	switch (message) {
		case WM_INITDIALOG:
			slew_window = hDlg;
			SendMessage(GetDlgItem(hDlg, IDC_OUTPUT), WM_SETFONT, (WPARAM)CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Courier New"), TRUE);
			SetDlgItemTextA(hDlg, IDC_OUTPUT, "");
			return (INT_PTR)TRUE;
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_PATH: {
					char path[MAX_PATH];
					char dir[MAX_PATH];
					TCHAR tpath[MAX_PATH] = { 0 };

					OPENFILENAME name;
					memset(&name, 0, sizeof(OPENFILENAME));
					name.lStructSize = sizeof(OPENFILENAME);
					name.hwndOwner = NULL;
					name.lpstrFilter = TEXT("All\0");
					name.lpstrFile = tpath;
					name.nMaxFile = MAX_PATH;
					name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					name.lpstrDefExt = TEXT("All\0");

					if (GetOpenFileName(&name)) {
						wcstombs(path, tpath, MAX_PATH);
						GetTempPathA(MAX_PATH, dir);
						strcat(dir, "megem.slew_path");
						FILE *file = fopen(dir, "wb");
						if (file) {
							fwrite(path, MAX_PATH, 1, file);
							fclose(file);
						}
					}

					break;
				}
				case IDC_STOP:
					if (thread) {
						TerminateThread(thread, 0);
						thread = 0;
						ResetStack();
					}
					break;
				case IDC_RUN: RUN: {
					static char path[MAX_PATH];
					char dir[MAX_PATH];

					GetTempPathA(MAX_PATH, dir);
					strcat(dir, "megem.slew_path");
					FILE *file = fopen(dir, "rb");
					if (file) {
						fread(path, MAX_PATH, 1, file);
						fclose(file);
						SetDlgItemTextA(hDlg, IDC_OUTPUT, "");
						if (thread) {
							TerminateThread(thread, 0);
							ResetStack();
						}
						thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SlewMain, path, 0, 0);
					} else {
						TCHAR tpath[MAX_PATH] = { 0 };

						OPENFILENAME name;
						memset(&name, 0, sizeof(OPENFILENAME));
						name.lStructSize = sizeof(OPENFILENAME);
						name.hwndOwner = NULL;
						name.lpstrFilter = TEXT("All\0");
						name.lpstrFile = tpath;
						name.nMaxFile = MAX_PATH;
						name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
						name.lpstrDefExt = TEXT("All\0");

						if (GetOpenFileName(&name)) {
							wcstombs(path, tpath, MAX_PATH);
							GetTempPathA(MAX_PATH, dir);
							strcat(dir, "megem.slew_path");
							FILE *file = fopen(dir, "wb");
							if (file) {
								fwrite(path, MAX_PATH, 1, file);
								fclose(file);
							}
							goto RUN;
						}
					}

					break;
				}
			}
			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			exit(0);
			return (INT_PTR)TRUE;
	}
	
	return (INT_PTR)FALSE;
}

int __cdecl printfHook(const char *_Format, ...) {
	va_list args;
	va_start(args, _Format);
	FILE *fnul = fopen("NUL", "w");
	int length = vfprintf(fnul, _Format, args);
	fclose(fnul);
	va_end(args);

	char *buffer = (char *)calloc(length + 1, 1);
	va_start(args, _Format);
	vsprintf(buffer, _Format, args);
	va_end(args);
	buffer[length] = 0;

	if (slew_window) {
		EditAppendText(GetDlgItem(slew_window, IDC_OUTPUT), buffer);
	}

	free(buffer);

	return length;
}

int __cdecl putsHook(const char *_Buffer) {
	if (slew_window) {
		EditAppendText(GetDlgItem(slew_window, IDC_OUTPUT), (char *)_Buffer);
	}
	return 0;
}

int __cdecl putcharHook(char _Character) {
	if (slew_window) {
		char buffer[2];
		buffer[0] = _Character;
		buffer[1] = 0;
		EditAppendText(GetDlgItem(slew_window, IDC_OUTPUT), (char *)buffer);
	}
	return _Character;
}

void SlewMain(char *path) {
	if (!GetProcess()) {
		return;
	}

	if (!stdout_hooked) {
		SetJMP(printfHook, printf, 0);
		SetJMP(putsHook, puts, 0);
		SetJMP(putcharHook, putchar, 0);
		stdout_hooked = true;
	}

	ResetStack();
	SetupLibraries();

	FILE *file = fopen(path, "rb");
	if (!file) {
		printf("error: unable to open '%s'", path);
		return;
	}
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = (char *)calloc(length + 1, 1);
	fread(buffer, length, 1, file);
	fclose(file);

	HWND hWnd = GetGameWindow();
	SetForegroundWindow(hWnd);
	SetActiveWindow(hWnd);
	SetFocus(hWnd);

	ARRAY *tokens = GetTokens(buffer);
	if (tokens) {
		unsigned int i = 0;
		for (; i < tokens->length; ++i) {
			TREE *tree = CreateTree(tokens, &i);

			if (tree) {
				VALUE value = Eval(tree, 0);
				Value_Free(&value);
				free(tree);
			}
		}
		for (i = 0; i < tokens->length; ++i) {
			free(((TOKEN *)Array_Get(tokens, i))->value);
		}
		Array_Free(tokens);
	}
}

VALUE *player, *camera;

VALUE player_jump(ARRAY *arguments) {
	HWND window = GetGameWindow();

	SendMessage(window, WM_KEYDOWN, VK_SPACE, VK_SPACE);
	SendMessage(window, WM_KEYUP, VK_SPACE, VK_SPACE);

	return Value_Number(0);
}

VALUE player_attack(ARRAY *arguments) {
	HWND window = GetGameWindow();

	SendMessage(window, WM_LBUTTONDOWN, MK_LBUTTON, MK_LBUTTON);
	SendMessage(window, WM_LBUTTONUP, MK_LBUTTON, MK_LBUTTON);

	return Value_Number(0);
}

VALUE player_grab(ARRAY *arguments) {
	HWND window = GetGameWindow();

	SendMessage(window, WM_RBUTTONDOWN, MK_RBUTTON, MK_RBUTTON);
	SendMessage(window, WM_RBUTTONUP, MK_RBUTTON, MK_RBUTTON);

	return Value_Number(0);
}

VALUE player_kg(ARRAY *arguments) {
	CallFunction("EXPORT_KG", 0);

	return Value_Number(0);
}
VALUE player_beamer(ARRAY *arguments) {
	CallFunction("EXPORT_Beamer", 0);

	return Value_Number(0);
}


VALUE player_strang(ARRAY *arguments) {
	CallFunction("EXPORT_Strang", 0);

	return Value_Number(0);
}

VALUE player_state_get(ARRAY *arguments) {
	char state = ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_STATE));
	Value_GetProperty(player, "state")->number = state;
	return Value_Number(state);
}

VALUE player_state_set(ARRAY *arguments) {
	char state = (char)Value_GetProperty(player, "state")->number;
	WriteChar(GetProcess(), (void *)(GetData().player_base + PLAYER_STATE), state);
	return Value_Number(state);
}

VALUE player_movement_state_get(ARRAY *arguments) {
	char movement_state = ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_MOVEMENT_STATE));
	Value_GetProperty(player, "movement_state")->number = movement_state;
	return Value_Number(movement_state);
}

VALUE player_walking_state_get(ARRAY *arguments) {
	char walking_state = ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_WALKING_STATE));
	Value_GetProperty(player, "walking_state")->number = walking_state;
	return Value_Number(walking_state);
}

VALUE player_walking_state_set(ARRAY *arguments) {
	char walking_state = (char)Value_GetProperty(player, "walking_state")->number;
	WriteChar(GetProcess(), (void *)(GetData().player_base + PLAYER_WALKING_STATE), walking_state);
	return Value_Number(walking_state);
}

VALUE player_movement_state_set(ARRAY *arguments) {
	char movement_state = (char)Value_GetProperty(player, "movement_state")->number;
	WriteChar(GetProcess(), (void *)(GetData().player_base + PLAYER_MOVEMENT_STATE), movement_state);
	return Value_Number(movement_state);
}

VALUE player_action_state_get(ARRAY *arguments) {
	char action_state = ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_ACTION_STATE));
	Value_GetProperty(player, "action_state")->number = action_state;
	return Value_Number(action_state);
}

VALUE player_action_state_set(ARRAY *arguments) {
	char action_state = (char)Value_GetProperty(player, "action_state")->number;
	WriteChar(GetProcess(), (void *)(GetData().player_base + PLAYER_ACTION_STATE), action_state);
	return Value_Number(action_state);
}

VALUE player_hand_state_get(ARRAY *arguments) {
	char hand_state = ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_HAND_STATE));
	Value_GetProperty(player, "hand_state")->number = hand_state;
	return Value_Number(hand_state);
}

VALUE player_hand_state_set(ARRAY *arguments) {
	char hand_state = (char)Value_GetProperty(player, "hand_state")->number;
	WriteChar(GetProcess(), (void *)(GetData().player_base + PLAYER_HAND_STATE), hand_state);
	return Value_Number(hand_state);
}

VALUE player_x_get(ARRAY *arguments) {
	float x = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X));
	Value_GetProperty(player, "x")->number = x;
	return Value_Number(x);
}

VALUE player_x_set(ARRAY *arguments) {
	float x = (float)Value_GetProperty(player, "x")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X), x);
	return Value_Number(x);
}

VALUE player_y_get(ARRAY *arguments) {
	float y = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y));
	Value_GetProperty(player, "y")->number = y;
	return Value_Number(y);
}

VALUE player_y_set(ARRAY *arguments) {
	float y = (float)Value_GetProperty(player, "y")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y), y);
	return Value_Number(y);
}

VALUE player_z_get(ARRAY *arguments) {
	float z = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Z));
	Value_GetProperty(player, "z")->number = z;
	return Value_Number(z);
}

VALUE player_z_set(ARRAY *arguments) {
	float z = (float)Value_GetProperty(player, "z")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Z), z);
	return Value_Number(z);
}

VALUE player_vx_get(ARRAY *arguments) {
	float vx = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VX));
	Value_GetProperty(player, "vx")->number = vx;
	return Value_Number(vx);
}

VALUE player_vx_set(ARRAY *arguments) {
	float vx = (float)Value_GetProperty(player, "vx")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VX), vx);
	return Value_Number(vx);
}

VALUE player_vy_get(ARRAY *arguments) {
	float vy = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VY));
	Value_GetProperty(player, "vy")->number = vy;
	return Value_Number(vy);
}

VALUE player_vy_set(ARRAY *arguments) {
	float vy = (float)Value_GetProperty(player, "vy")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VY), vy);
	return Value_Number(vy);
}

VALUE player_vz_get(ARRAY *arguments) {
	float vz = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VZ));
	Value_GetProperty(player, "vz")->number = vz;
	return Value_Number(vz);
}

VALUE player_vz_set(ARRAY *arguments) {
	float vz = (float)Value_GetProperty(player, "vz")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VZ), vz);
	return Value_Number(vz);
}

VALUE player_rx_get(ARRAY *arguments) {
	DWORD rx = ReadInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RX));
	Value_GetProperty(player, "rx")->number = IntToDegrees(rx);
	return Value_Number(rx);
}

VALUE player_rx_set(ARRAY *arguments) {
	float rx = (float)Value_GetProperty(player, "rx")->number;
	WriteInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RX), DegreesToInt(rx));
	return Value_Number(rx);
}

VALUE player_ry_get(ARRAY *arguments) {
	DWORD ry = ReadInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RY));
	Value_GetProperty(player, "ry")->number = IntToDegrees(ry);
	return Value_Number(ry);
}

VALUE player_ry_set(ARRAY *arguments) {
	float ry = (float)Value_GetProperty(player, "ry")->number;
	WriteInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RY), DegreesToInt(ry));
	return Value_Number(ry);
}

VALUE player_rz_get(ARRAY *arguments) {
	DWORD rz = ReadInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RZ));
	Value_GetProperty(player, "rz")->number = IntToDegrees(rz);
	return Value_Number(rz);
}

VALUE player_rz_set(ARRAY *arguments) {
	float rz = (float)Value_GetProperty(player, "rz")->number;
	WriteInt(GetProcess(), (void *)(GetData().player_base + PLAYER_RZ), DegreesToInt(rz));
	return Value_Number(rz);
}

VALUE player_scale_get(ARRAY *arguments) {
	float scale = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SCALE));
	Value_GetProperty(player, "scale")->number = scale;
	return Value_Number(scale);
}

VALUE player_scale_set(ARRAY *arguments) {
	float scale = (float)Value_GetProperty(player, "scale")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SCALE), scale);
	return Value_Number(scale);
}

VALUE player_sx_get(ARRAY *arguments) {
	float sx = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SX));
	Value_GetProperty(player, "sx")->number = sx;
	return Value_Number(sx);
}

VALUE player_sx_set(ARRAY *arguments) {
	float sx = (float)Value_GetProperty(player, "sx")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SX), sx);
	return Value_Number(sx);
}

VALUE player_sy_get(ARRAY *arguments) {
	float sy = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SY));
	Value_GetProperty(player, "sy")->number = sy;
	return Value_Number(sy);
}

VALUE player_sy_set(ARRAY *arguments) {
	float sy = (float)Value_GetProperty(player, "sy")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SY), sy);
	return Value_Number(sy);
}

VALUE player_sz_get(ARRAY *arguments) {
	float sz = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SZ));
	Value_GetProperty(player, "sz")->number = sz;
	return Value_Number(sz);
}

VALUE player_sz_set(ARRAY *arguments) {
	float sz = (float)Value_GetProperty(player, "sz")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SZ), sz);
	return Value_Number(sz);
}

VALUE player_health_get(ARRAY *arguments) {
	DWORD health = ReadInt(GetProcess(), (void *)(GetData().player_base + PLAYER_HEALTH));
	Value_GetProperty(player, "health")->number = health;
	return Value_Number(health);
}

VALUE player_health_set(ARRAY *arguments) {
	DWORD health = (DWORD)Value_GetProperty(player, "health")->number;
	WriteInt(GetProcess(), (void *)(GetData().player_base + PLAYER_HEALTH, health), health);
	return Value_Number(health);
}

VALUE player_max_health_get(ARRAY *arguments) {
	DWORD max_health = ReadInt(GetProcess(), (void *)(GetData().player_base + PLAYER_MAX_HEALTH));
	Value_GetProperty(player, "max_health")->number = max_health;
	return Value_Number(max_health);
}

VALUE player_max_health_set(ARRAY *arguments) {
	DWORD max_health = (DWORD)Value_GetProperty(player, "max_health")->number;
	WriteInt(GetProcess(), (void *)(GetData().player_base + PLAYER_MAX_HEALTH), max_health);
	return Value_Number(max_health);
}

VALUE player_max_ground_speed_get(ARRAY *arguments) {
	float max_ground_speed = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_MAX_GROUND_SPEED));
	Value_GetProperty(player, "max_ground_speed")->number = max_ground_speed;
	return Value_Number(max_ground_speed);
}

VALUE player_max_ground_speed_set(ARRAY *arguments) {
	float max_ground_speed = (float)Value_GetProperty(player, "max_ground_speed")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_MAX_GROUND_SPEED), max_ground_speed);
	return Value_Number(max_ground_speed);
}

VALUE player_lgx_get(ARRAY *arguments) {
	float lgx = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGX));
	Value_GetProperty(player, "lgx")->number = lgx;
	return Value_Number(lgx);
}

VALUE player_lgx_set(ARRAY *arguments) {
	float lgx = (float)Value_GetProperty(player, "lgx")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGX), lgx);
	return Value_Number(lgx);
}

VALUE player_lgy_get(ARRAY *arguments) {
	float lgy = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGY));
	Value_GetProperty(player, "lgy")->number = lgy;
	return Value_Number(lgy);
}

VALUE player_lgy_set(ARRAY *arguments) {
	float lgy = (float)Value_GetProperty(player, "lgy")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGY), lgy);
	return Value_Number(lgy);
}

VALUE player_lgz_get(ARRAY *arguments) {
	float lgz = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGZ));
	Value_GetProperty(player, "lgz")->number = lgz;
	return Value_Number(lgz);
}

VALUE player_lgz_set(ARRAY *arguments) {
	float lgz = (float)Value_GetProperty(player, "lgz")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_LGZ), lgz);
	return Value_Number(lgz);
}

VALUE player_ox_get(ARRAY *arguments) {
	float ox = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OX));
	Value_GetProperty(player, "ox")->number = ox;
	return Value_Number(ox);
}

VALUE player_ox_set(ARRAY *arguments) {
	float ox = (float)Value_GetProperty(player, "ox")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OX), ox);
	return Value_Number(ox);
}

VALUE player_oy_get(ARRAY *arguments) {
	float oy = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OY));
	Value_GetProperty(player, "oy")->number = oy;
	return Value_Number(oy);
}

VALUE player_oy_set(ARRAY *arguments) {
	float oy = (float)Value_GetProperty(player, "oy")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OY), oy);
	return Value_Number(oy);
}

VALUE player_oz_get(ARRAY *arguments) {
	float oz = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OZ));
	Value_GetProperty(player, "oz")->number = oz;
	return Value_Number(oz);
}

VALUE player_oz_set(ARRAY *arguments) {
	float oz = (float)Value_GetProperty(player, "oz")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_OZ), oz);
	return Value_Number(oz);
}

VALUE player_fz_get(ARRAY *arguments) {
	float fz = ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_FZ));
	Value_GetProperty(player, "fz")->number = fz;
	return Value_Number(fz);
}

VALUE player_fz_set(ARRAY *arguments) {
	float fz = (float)Value_GetProperty(player, "fz")->number;
	WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_FZ), fz);
	return Value_Number(fz);
}

VALUE player_reaction_time_get(ARRAY *arguments) {
	float reaction_time = ReadFloat(GetProcess(), (void *)(GetData().camera_base + CAMERA_REACTION_TIME));
	Value_GetProperty(player, "reaction_time")->number = reaction_time;
	return Value_Number(reaction_time);
}

VALUE player_reaction_time_set(ARRAY *arguments) {
	float reaction_time = (float)Value_GetProperty(player, "reaction_time")->number;
	WriteFloat(GetProcess(), (void *)(GetData().camera_base + CAMERA_REACTION_TIME), reaction_time);
	return Value_Number(reaction_time);
}

VALUE camera_rx_get(ARRAY *arguments) {
	DWORD rx = ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RX));
	Value_GetProperty(camera, "rx")->number = IntToDegrees(rx);
	return Value_Number(rx);
}

VALUE camera_rx_set(ARRAY *arguments) {
	float rx = (float)Value_GetProperty(camera, "rx")->number;
	WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RX), DegreesToInt(rx));
	return Value_Number(rx);
}

VALUE camera_ry_get(ARRAY *arguments) {
	DWORD ry = ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY));
	Value_GetProperty(camera, "ry")->number = IntToDegrees(ry);
	return Value_Number(ry);
}

VALUE camera_ry_set(ARRAY *arguments) {
	float ry = (float)Value_GetProperty(camera, "ry")->number;
	WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), DegreesToInt(ry));
	return Value_Number(ry);
}

VALUE camera_rz_get(ARRAY *arguments) {
	DWORD rz = ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RZ));
	Value_GetProperty(camera, "rz")->number = IntToDegrees(rz);
	return Value_Number(rz);
}

VALUE camera_rz_set(ARRAY *arguments) {
	float rz = (float)Value_GetProperty(camera, "rz")->number;
	WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RZ), DegreesToInt(rz));
	return Value_Number(rz);
}

VALUE player_waitForState(ARRAY *arguments) {
	if (arguments->length > 0) {
		while ((DWORD)ReadChar(GetProcess(), (void *)(GetData().player_base + PLAYER_STATE)) != (char)((VALUE *)Array_Get(arguments, 0))->number) {
			Sleep(1);
		}
	}
	return Value_Number(0);
}

VALUE player_sideJumpRight(ARRAY *arguments) {
	HWND window = GetGameWindow();

	Keydown(GetKeybinds()->fly_right);
	SendMessage(window, WM_KEYDOWN, VK_SPACE, VK_SPACE);
	SendMessage(window, WM_KEYUP, VK_SPACE, VK_SPACE);

	long long frame = GetData().frame + 1;
	while (GetData().frame <= frame) {
		Sleep(1);
	}

	Keyup(GetKeybinds()->fly_right);

	return Value_Number(0);
}

VALUE player_sideJumpLeft(ARRAY *arguments) {
	HWND window = GetGameWindow();

	Keydown(GetKeybinds()->fly_left);
	SendMessage(window, WM_KEYDOWN, VK_SPACE, VK_SPACE);
	SendMessage(window, WM_KEYUP, VK_SPACE, VK_SPACE);

	long long frame = GetData().frame + 1;
	while (GetData().frame <= frame) {
		Sleep(1);
	}

	Keyup(GetKeybinds()->fly_left);

	return Value_Number(0);
}

VALUE wait(ARRAY *arguments) {
	if (arguments->length > 0) {
		Sleep((DWORD)((VALUE *)Array_Get(arguments, 0))->number);
	}
	return Value_Number(0);
}

VALUE waitFrame(ARRAY *arguments) {
	long long offset = 1;
	if (arguments->length > 0) {
		offset = (long long)((VALUE *)Array_Get(arguments, 0))->number;
	}

	long long frame = GetData().frame + offset;
	while (GetData().frame <= frame) {
		Sleep(1);
	}

	return Value_Number(0);
}

VALUE player_forward(ARRAY *arguments) {
	if (arguments->length > 0) {
		if (((VALUE *)Array_Get(arguments, 0))->number) {
			Keydown(GetKeybinds()->fly_forward);
		} else {
			Keyup(GetKeybinds()->fly_forward);
		}
	} else {
		Keydown(GetKeybinds()->fly_forward);
	}

	return Value_Number(0);
}

VALUE player_backward(ARRAY *arguments) {
	if (arguments->length > 0) {
		if (((VALUE *)Array_Get(arguments, 0))->number) {
			Keydown(GetKeybinds()->fly_backward);
		} else {
			Keyup(GetKeybinds()->fly_backward);
		}
	} else {
		Keydown(GetKeybinds()->fly_backward);
	}

	return Value_Number(0);
}

VALUE player_left(ARRAY *arguments) {
	if (arguments->length > 0) {
		if (((VALUE *)Array_Get(arguments, 0))->number) {
			Keydown(GetKeybinds()->fly_left);
		} else {
			Keyup(GetKeybinds()->fly_left);
		}
	} else {
		Keydown(GetKeybinds()->fly_left);
	}

	return Value_Number(0);
}

VALUE player_right(ARRAY *arguments) {
	if (arguments->length > 0) {
		if (((VALUE *)Array_Get(arguments, 0))->number) {
			Keydown(GetKeybinds()->fly_right);
		} else {
			Keyup(GetKeybinds()->fly_right);
		}
	} else {
		Keydown(GetKeybinds()->fly_right);
	}

	return Value_Number(0);
}

float EaseInOut(float t, int b, int c, float d) {
	return (float)(-c / 2 * (cos(PI*t / d) - 1) + b);
}

VALUE camera_turn(ARRAY *arguments) {
	if (arguments->length == 1) {
		WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), DegreesToInt((float)(((VALUE *)Array_Get(arguments, 0))->number)));
	} else if (arguments->length > 1) {
		DWORD start = (ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY)) % 0x10000) + 0x10000;
		DWORD change = (DegreesToInt((float)(((VALUE *)Array_Get(arguments, 0))->number))) + 0x10000;

		while (change < start) {
			change += 0x10000;
		}

		if (abs((int)((change - 0x10000) - start)) < abs((int)(change - start))) {
			change -= 0x10000;
		}

		change -= start;

		int ms = (int)(((VALUE *)Array_Get(arguments, 1))->number);
		for (int i = 0; i < ms; ++i) {
			WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), (int)EaseInOut((float)i / ms, start, change, 1));
			Sleep(1);
		}
	}

	return Value_Number(0);
}

VALUE player_sideJump(ARRAY *arguments) {
	if (arguments->length > 1) {
		int ms = (int)(((VALUE *)Array_Get(arguments, 1))->number);
		int start = (ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY)) % 0x10000) + 0x10000;
		bool left = (((VALUE *)Array_Get(arguments, 0))->number) < 0;
		int change = left ? -0x4000 : 0x4000;

		for (int i = 0; i < ms; ++i) {
			WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), (int)EaseInOut((float)i / ms, start, change, 1));
			Sleep(1);
		}

		if (left) {
			player_sideJumpRight(NULL);
		} else {
			player_sideJumpLeft(NULL);
		}

		start += change;
		change = -change;
		for (int i = 0; i < ms; ++i) {
			WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), (int)EaseInOut((float)i / ms, start, change, 1));
			Sleep(1);
		}
	}

	return Value_Number(0);
}

VALUE player_setPosition(ARRAY *arguments) {
	if (arguments->length == 2) {
		WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X), (float)(((VALUE *)Array_Get(arguments, 0))->number));
		WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y), (float)(((VALUE *)Array_Get(arguments, 1))->number));
	} else if (arguments->length > 2) {
		WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X), (float)(((VALUE *)Array_Get(arguments, 0))->number));
		WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y), (float)(((VALUE *)Array_Get(arguments, 1))->number));
		WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Z), (float)(((VALUE *)Array_Get(arguments, 2))->number));
	}

	return Value_Number(0);
}

VALUE camera_setRotation(ARRAY *arguments) {
	if (arguments->length > 1) {
		WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RX), DegreesToInt((float)(((VALUE *)Array_Get(arguments, 0))->number)));
		WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), DegreesToInt((float)(((VALUE *)Array_Get(arguments, 1))->number)));
	}

	return Value_Number(0);
}

VALUE player_crouch(ARRAY *arguments) {
	if (arguments->length > 0) {
		if (((VALUE *)Array_Get(arguments, 0))->number) {
			Keydown(VK_LSHIFT);
		} else {
			Keyup(VK_LSHIFT);
		}
	} else {
		Keydown(VK_LSHIFT);
	}

	return Value_Number(0);
}

void MirrorsEdgeLibrary() {
	Stack_Push("wait", &Value_Compiled_Function(&wait), 0);
	Stack_Push("waitFrame", &Value_Compiled_Function(&waitFrame), 0);

	player = Stack_Push("player", &Value_Number(GetData().player_base), 0);

	Value_SetProperty(player, "STATE_HANGING", &Value_Number(0));
	Value_SetProperty(player, "STATE_GROUNDED", &Value_Number(1));
	Value_SetProperty(player, "STATE_IN_AIR", &Value_Number(2));
	Value_SetProperty(player, "STATE_ANIMATION", &Value_Number(4));
	Value_SetProperty(player, "STATE_WALLRUN", &Value_Number(12));
	Value_SetProperty(player, "STATE_WALLCLIMB", &Value_Number(13));

	Value_SetProperty(player, "jump", &Value_Compiled_Function(&player_jump));
	Value_SetProperty(player, "attack", &Value_Compiled_Function(&player_attack));
	Value_SetProperty(player, "grab", &Value_Compiled_Function(&player_grab));
	Value_SetProperty(player, "kg", &Value_Compiled_Function(&player_kg));
	Value_SetProperty(player, "beamer", &Value_Compiled_Function(&player_beamer));
	Value_SetProperty(player, "strang", &Value_Compiled_Function(&player_strang));
	Value_SetProperty(player, "waitForState", &Value_Compiled_Function(&player_waitForState));
	Value_SetProperty(player, "sideJumpRight", &Value_Compiled_Function(&player_sideJumpRight));
	Value_SetProperty(player, "forward", &Value_Compiled_Function(&player_forward));
	Value_SetProperty(player, "backward", &Value_Compiled_Function(&player_backward));
	Value_SetProperty(player, "left", &Value_Compiled_Function(&player_left));
	Value_SetProperty(player, "right", &Value_Compiled_Function(&player_right));
	Value_SetProperty(player, "sideJump", &Value_Compiled_Function(&player_sideJump));
	Value_SetProperty(player, "setPosition", &Value_Compiled_Function(&player_setPosition));
	Value_SetProperty(player, "crouch", &Value_Compiled_Function(&player_crouch));

	VALUE *state = Value_SetProperty(player, "state", &Value_Number(0));
	Value_SetProperty(state, "get", &Value_Compiled_Function(player_state_get));
	Value_SetProperty(state, "set", &Value_Compiled_Function(player_state_set));
	VALUE *movement_state = Value_SetProperty(player, "movement_state", &Value_Number(0));
	Value_SetProperty(movement_state, "get", &Value_Compiled_Function(player_movement_state_get));
	Value_SetProperty(movement_state, "set", &Value_Compiled_Function(player_movement_state_set));
	VALUE *walking_state = Value_SetProperty(player, "walking_state", &Value_Number(0));
	Value_SetProperty(walking_state, "get", &Value_Compiled_Function(player_walking_state_get));
	Value_SetProperty(walking_state, "set", &Value_Compiled_Function(player_walking_state_set));
	VALUE *action_state = Value_SetProperty(player, "action_state", &Value_Number(0));
	Value_SetProperty(action_state, "get", &Value_Compiled_Function(player_action_state_get));
	Value_SetProperty(action_state, "set", &Value_Compiled_Function(player_action_state_set));
	VALUE *hand_state = Value_SetProperty(player, "hand_state", &Value_Number(0));
	Value_SetProperty(hand_state, "get", &Value_Compiled_Function(player_hand_state_get));
	Value_SetProperty(hand_state, "set", &Value_Compiled_Function(player_hand_state_set));
	VALUE *x = Value_SetProperty(player, "x", &Value_Number(0));
	Value_SetProperty(x, "get", &Value_Compiled_Function(player_x_get));
	Value_SetProperty(x, "set", &Value_Compiled_Function(player_x_set));
	VALUE *y = Value_SetProperty(player, "y", &Value_Number(0));
	Value_SetProperty(y, "get", &Value_Compiled_Function(player_y_get));
	Value_SetProperty(y, "set", &Value_Compiled_Function(player_y_set));
	VALUE *z = Value_SetProperty(player, "z", &Value_Number(0));
	Value_SetProperty(z, "get", &Value_Compiled_Function(player_z_get));
	Value_SetProperty(z, "set", &Value_Compiled_Function(player_z_set));
	VALUE *vx = Value_SetProperty(player, "vx", &Value_Number(0));
	Value_SetProperty(vx, "get", &Value_Compiled_Function(player_vx_get));
	Value_SetProperty(vx, "set", &Value_Compiled_Function(player_vx_set));
	VALUE *vy = Value_SetProperty(player, "vy", &Value_Number(0));
	Value_SetProperty(vy, "get", &Value_Compiled_Function(player_vy_get));
	Value_SetProperty(vy, "set", &Value_Compiled_Function(player_vy_set));
	VALUE *vz = Value_SetProperty(player, "vz", &Value_Number(0));
	Value_SetProperty(vz, "get", &Value_Compiled_Function(player_vz_get));
	Value_SetProperty(vz, "set", &Value_Compiled_Function(player_vz_set));
	VALUE *rx = Value_SetProperty(player, "rx", &Value_Number(0));
	Value_SetProperty(rx, "get", &Value_Compiled_Function(player_rx_get));
	Value_SetProperty(rx, "set", &Value_Compiled_Function(player_rx_set));
	VALUE *ry = Value_SetProperty(player, "ry", &Value_Number(0));
	Value_SetProperty(ry, "get", &Value_Compiled_Function(player_ry_get));
	Value_SetProperty(ry, "set", &Value_Compiled_Function(player_ry_set));
	VALUE *rz = Value_SetProperty(player, "rz", &Value_Number(0));
	Value_SetProperty(rz, "get", &Value_Compiled_Function(player_rz_get));
	Value_SetProperty(rz, "set", &Value_Compiled_Function(player_rz_set));
	VALUE *scale = Value_SetProperty(player, "scale", &Value_Number(0));
	Value_SetProperty(scale, "get", &Value_Compiled_Function(player_scale_get));
	Value_SetProperty(scale, "set", &Value_Compiled_Function(player_scale_set));
	VALUE *sx = Value_SetProperty(player, "sx", &Value_Number(0));
	Value_SetProperty(sx, "get", &Value_Compiled_Function(player_sx_get));
	Value_SetProperty(sx, "set", &Value_Compiled_Function(player_sx_set));
	VALUE *sy = Value_SetProperty(player, "sy", &Value_Number(0));
	Value_SetProperty(sy, "get", &Value_Compiled_Function(player_sy_get));
	Value_SetProperty(sy, "set", &Value_Compiled_Function(player_sy_set));
	VALUE *sz = Value_SetProperty(player, "sz", &Value_Number(0));
	Value_SetProperty(sz, "get", &Value_Compiled_Function(player_sz_get));
	Value_SetProperty(sz, "set", &Value_Compiled_Function(player_sz_set));
	VALUE *max_ground_speed = Value_SetProperty(player, "max_ground_speed", &Value_Number(0));
	Value_SetProperty(max_ground_speed, "get", &Value_Compiled_Function(player_max_ground_speed_get));
	Value_SetProperty(max_ground_speed, "set", &Value_Compiled_Function(player_max_ground_speed_set));
	VALUE *health = Value_SetProperty(player, "health", &Value_Number(0));
	Value_SetProperty(health, "get", &Value_Compiled_Function(player_health_get));
	Value_SetProperty(health, "set", &Value_Compiled_Function(player_health_set));
	VALUE *max_health = Value_SetProperty(player, "max_health", &Value_Number(0));
	Value_SetProperty(max_health, "get", &Value_Compiled_Function(player_max_health_get));
	Value_SetProperty(max_health, "set", &Value_Compiled_Function(player_max_health_set));
	VALUE *lgx = Value_SetProperty(player, "lgx", &Value_Number(0));
	Value_SetProperty(lgx, "get", &Value_Compiled_Function(player_lgx_get));
	Value_SetProperty(lgx, "set", &Value_Compiled_Function(player_lgx_set));
	VALUE *lgy = Value_SetProperty(player, "lgy", &Value_Number(0));
	Value_SetProperty(lgy, "get", &Value_Compiled_Function(player_lgy_get));
	Value_SetProperty(lgy, "set", &Value_Compiled_Function(player_lgy_set));
	VALUE *lgz = Value_SetProperty(player, "lgz", &Value_Number(0));
	Value_SetProperty(lgz, "get", &Value_Compiled_Function(player_lgz_get));
	Value_SetProperty(lgz, "set", &Value_Compiled_Function(player_lgz_set));
	VALUE *ox = Value_SetProperty(player, "ox", &Value_Number(0));
	Value_SetProperty(ox, "get", &Value_Compiled_Function(player_ox_get));
	Value_SetProperty(ox, "set", &Value_Compiled_Function(player_ox_set));
	VALUE *oy = Value_SetProperty(player, "oy", &Value_Number(0));
	Value_SetProperty(oy, "get", &Value_Compiled_Function(player_oy_get));
	Value_SetProperty(oy, "set", &Value_Compiled_Function(player_oy_set));
	VALUE *oz = Value_SetProperty(player, "oz", &Value_Number(0));
	Value_SetProperty(oz, "get", &Value_Compiled_Function(player_oz_get));
	Value_SetProperty(oz, "set", &Value_Compiled_Function(player_oz_set));
	VALUE *fz = Value_SetProperty(player, "fz", &Value_Number(0));
	Value_SetProperty(fz, "get", &Value_Compiled_Function(player_fz_get));
	Value_SetProperty(fz, "set", &Value_Compiled_Function(player_fz_set));
	VALUE *reaction_time = Value_SetProperty(player, "reaction_time", &Value_Number(0));
	Value_SetProperty(reaction_time, "get", &Value_Compiled_Function(player_reaction_time_get));
	Value_SetProperty(reaction_time, "set", &Value_Compiled_Function(player_reaction_time_set));

	camera = Stack_Push("camera", &Value_Number(0), 0);
	rx = Value_SetProperty(camera, "rx", &Value_Number(0));
	Value_SetProperty(rx, "get", &Value_Compiled_Function(camera_rx_get));
	Value_SetProperty(rx, "set", &Value_Compiled_Function(camera_rx_set));
	ry = Value_SetProperty(camera, "ry", &Value_Number(0));
	Value_SetProperty(ry, "get", &Value_Compiled_Function(camera_ry_get));
	Value_SetProperty(ry, "set", &Value_Compiled_Function(camera_ry_set));
	rz = Value_SetProperty(camera, "rz", &Value_Number(0));
	Value_SetProperty(rz, "get", &Value_Compiled_Function(camera_rz_get));
	Value_SetProperty(rz, "set", &Value_Compiled_Function(camera_rz_set));
	Value_SetProperty(camera, "turn", &Value_Compiled_Function(&camera_turn));
	Value_SetProperty(camera, "setRotation", &Value_Compiled_Function(&camera_setRotation));
}