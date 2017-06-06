#include "stdafx.h"

DWORD engine_base = 0;
DWORD fall_effect_base = 0;
HANDLE engine_console = 0;
bool kg = false, god = false, strang = false, beamer = false;
bool float_ = false;
float float_x, float_y, float_z, float_vx, float_vy, float_vz, float_speed;
DWORD loading_addr;
bool static_load = false;
char *zero = (char *)calloc(0xFFF, 1);

void(*GetEngineBaseOriginal)();
int(__thiscall *UpdateEngineOriginal)(int this_);
DWORD LevelLoadBase = 0;
int(__thiscall *LevelLoadOriginal)(void *this_, int, __int64);
void **(__thiscall *DebugPrintOriginal)(void **, void *);
HMODULE(WINAPI *LoadLibraryAOriginal)(char *);

__declspec(naked) void GetEngineBaseHook() {
	__asm {
		mov engine_base, ecx
		push ecx
		push eax
	}

	GetData()->engine_base = engine_base;

	__asm {
		pop eax
		pop ecx
		jmp GetEngineBaseOriginal
	}
}

int __fastcall UpdateEngineHook(int this_, void *idle_) {
	static float last_speed = 0;
	static long long top_speed_frame = 0;

	GetData()->loading = Loading();

	if (!Loading()) {
		++(GetData()->frame);
		UpdateDolly();
		GetData()->fov = GetFOV();

		if (GetPlayerBase() && ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE)) == 20 && ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_FY)) == -2147483648.0) {
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 2);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 1);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
		}

		if (kg && GetPlayerBase()) {
			if (ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + 0x68)) == 1) {
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYDOWN, VK_SPACE, VK_SPACE);
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYUP, VK_SPACE, VK_SPACE);

				kg = false;
			} else if (ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + 0x68)) == 12) {
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_LBUTTONDOWN, MK_LBUTTON, MK_LBUTTON);
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_LBUTTONUP, MK_LBUTTON, MK_LBUTTON);
			}
		}

		if (strang && GetPlayerBase()) {
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MAX_GROUND_SPEED), 2147483648.0);

			if (ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE)) == 1) {
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYDOWN, VK_SPACE, VK_SPACE);
				SendMessage(FindWindowExA(0, 0, "LaunchUnrealUWindowsClient", 0), WM_KEYUP, VK_SPACE, VK_SPACE);

				WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MAX_GROUND_SPEED), 720);

				strang = false;
			}
		}

		if (beamer && GetPlayerBase()) {
			if (ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE)) == 1) {
				beamer = false;
			} else {
				float vx = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VX));
				float vy = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VY));
				if ((float)sqrt(vx * vx + vy * vy) > last_speed + 30) {
					SetForegroundWindow(GetGameWindow());
					Keydown(GetKeybinds()->quickturn);
					Keyup(GetKeybinds()->quickturn);
					beamer = false;
				}
			}
		}

		if ((god || float_) && GetPlayerBase() && strcmp(GetData()->level, "TdMainMenu") != 0) {
			WriteInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_LOCK), 0);
			WriteInt(GetCurrentProcess(), GetPointer(GetCurrentProcess(), 4, fall_effect_base, 0xCC, 0x4A8, 0x18C), 0);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_FY), -2147483648.0);
			WriteInt(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HEALTH), 100);

			if (ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE)) == 20) {
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 2);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 1);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
			}
		}

		if (float_ && GetPlayerBase() && ReadInt(GetCurrentProcess(), (void *)(GetEngineBase() + ENGINE_PAUSED)) == 0) {
			float r = IntToDegrees(ReadInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RY)));
			float hv = 0, vv = 0;

			if (IsGameWindow(GetForegroundWindow())) {
				bool forward = IsKeyDown(GetKeybinds()->fly_forward);
				bool backward = IsKeyDown(GetKeybinds()->fly_backward);
				bool left = IsKeyDown(GetKeybinds()->fly_left);
				bool right = IsKeyDown(GetKeybinds()->fly_right);
				bool up = IsKeyDown(VK_SPACE);
				bool down = IsKeyDown(VK_SHIFT);

				if (backward) r += 180;
				if (left) r -= backward ? -45 : forward ? 45 : 90;
				if (right) r += backward ? -45 : forward ? 45 : 90;

				if (left || right || forward || backward) {
					hv = (float)(float_speed * 0.1);
				}

				if (up) {
					vv = (float)(float_speed * 0.1);
				} else if (down) {
					vv = (float)(-float_speed * 0.1);
				}

				float_vx += (float)cos(r * PI / 180) * hv;
				float_vy += (float)sin(r * PI / 180) * hv;
				float_vz += vv;
			}

			float_x += float_vx;
			float_y += float_vy;
			float_z += float_vz;

			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_X), float_x);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Y), float_y);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Z), float_z);

			if (IsKeyDown(GetKeybinds()->fly_increase)) {
				float_speed++;
			} else if (IsKeyDown(GetKeybinds()->fly_decrease)) {
				float_speed--;
			}

			if (float_speed < 1) {
				float_speed = 1;
			} else if (float_speed > 250) {
				float_speed = 250;
			}

			hv = (float)sqrt(float_vx * float_vx + float_vy * float_vy);
			hv *= hv;

			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VX), (float)cos(r * PI / 180) * hv);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VY), (float)sin(r * PI / 180) * hv);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VZ), SIGN(float_vz) * (float_vz * float_vz));

			float_vx *= (float)0.9;
			float_vy *= (float)0.9;
			float_vz *= (float)0.9;

			if (strcmp(GetData()->level, "TdMainMenu") != 0) {
				WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_COLLISION), -105.578125);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 0);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 7);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
				WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
			}
		}

		if (GetPlayerBase()) {
			float vx = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VX));
			float vy = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VY));
			last_speed = (float)sqrt(vx * vx + vy * vy);
			if (last_speed > GetData()->top_speed) {
				GetData()->top_speed = last_speed;
				top_speed_frame = 0;
			}
			if (++top_speed_frame >= 200 || GetData()->top_speed * 0.036 >= 9969413) {
				GetData()->top_speed = 0;
			}
		}
	} else {
		beamer = kg = strang = false;
		GetData()->player_base = (DWORD)zero;
		GetData()->camera_base = (DWORD)zero;
		GetData()->top_speed = 0;
		top_speed_frame = 0;
	}

	return UpdateEngineOriginal(this_);
}

int __fastcall LevelLoadHook(void *this_, void *idle_, int a2, __int64 a3) {
	static_load = true;
	kg = beamer = strang = false;
	GetData()->player_base = (DWORD)zero;
	GetData()->camera_base = (DWORD)zero;
	GetData()->top_speed = 0;
	wchar_t *level_name = *(wchar_t **)(a2 + 0x1C);
	WCharToChar(GetData()->level, level_name);
	LevelLoadOriginal(this_, a2, a3);

	static_load = false;

	return 1;
}

/* __declspec(naked) void LevelLoadHook() {
	static wchar_t *level_name;

	__asm {
		push 0xFFFFFFFF
		push eax
		push ebx
		push ecx
		push edx
		push esi
		push edi
		push esp
		mov eax, [ecx + 0x1C]
		mov level_name, eax
	}

	static_load = true;
	/* beamer = kg = strang = false;
	GetData()->player_base = (DWORD)zero;
	GetData()->camera_base = (DWORD)zero;
	GetData()->top_speed = 0;
	WCharToChar(GetData()->level, level_name);
	GetData()->level[wcslen(level_name)] = 0;

	__asm {
		pop esp
		pop edi
		pop esi
		pop edx
		pop ecx
		pop ebx
		pop eax
		jmp LevelLoadOriginal
	}
} */

void **__fastcall DebugPrintHook(void **this_, void *idle_, void *src) {
	if (engine_console && src) {
		if (!wcsempty((wchar_t *)src) && !wcsstr((wchar_t *)src, L"..\\TdGame\\Config\\TdEngine.ini") && !wcsstr((wchar_t *)src, L"Engine.ISVHacks") && !wcsstr((wchar_t *)src, L"UseMinimalNVIDIADriverShaderOptimization") && !wcsstr((wchar_t *)src, L"CanvasObject")) {
			WriteConsole(engine_console, (wchar_t *)src, wcslen((wchar_t *)src), NULL, NULL);
			WriteConsole(engine_console, L"\n", 1, NULL, NULL);
		}
	}

	return DebugPrintOriginal(this_, src);
}

DWORD GetEngineBase() {
	return engine_base;
}

void KG(bool a) {
	kg = a;
}

void Strang(bool a) {
	strang = a;
}

void Beamer(bool a) {
	beamer = a;
}

void God(bool a) {
	if (a == god) {
		return;
	}

	god = a;
	if (god && GetPlayerBase() && ReadChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE)) == 72 && strcmp(GetData()->level, "TdMainMenu") != 0) {
		WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 2);
		WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 1);
		WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
		WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
		WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
	}
}

void Float(bool a) {
	if (a == float_) {
		return;
	}

	if (a) {
		float_speed = 30;
		float_vx = float_vy = float_vz = 0;
		float_x = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_X));
		float_y = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Y));
		float_z = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Z));
		float_ = a;
	} else {
		float_ = a;
		if (strcmp(GetData()->level, "TdMainMenu") != 0) {
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 2);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 1);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
		}
	}
}

void OpenDebugConsole() {
	AllocConsole();
	engine_console = GetStdHandle(STD_OUTPUT_HANDLE);
}

void CloseDebugConsole() {
	engine_console = 0;
	FreeConsole();
}

bool Loading() {
	return static_load || ReadInt(GetCurrentProcess(), (void *)loading_addr) == 0;
}

// Work around for Fatalis's autosplitter
void ReHookListener() {
	for (;;) {
		if (*(unsigned char *)LevelLoadBase == 0xE9) {
			TrampolineHook(LevelLoadHook, (void *)LevelLoadBase, (void **)&LevelLoadOriginal);
			return;
		}

		Sleep(1);
	}
}

HMODULE WINAPI LoadLibraryAHook(char *module) {
	if (strstr(module, "menl_hooks.dll")) {
		UnTrampolineHook((void *)LevelLoadBase, (void **)&LevelLoadOriginal);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ReHookListener, 0, 0, 0);
	}

	return LoadLibraryAOriginal(module);
}

void SetupEngine() {
	DWORD addr;

	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\xD9\xEE\x8B\x01\x8B\x50\x1C\x6A\x00\x6A\x01\x51\xD9\x1C\x24\xFF\xD2\x83\xA6\xF4\x04\x00\x00\xFA\xC7\x05", "xxxxxxxxxxxxxxxxxxxxxxxxxx");
	loading_addr = *(DWORD *)(addr + 26);

	fall_effect_base = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\x89\x0D\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xFF", "xx????x????x");
	fall_effect_base = *(DWORD *)(fall_effect_base + 0x2);

	// addr = 0xEF90C0;
	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\x56\x8B\xF1\x8B\x06\x8B\x90\x34\x01\x00\x00\xFF\xD2\xD8\x8E\xD8\x0C\x00\x00\x5E\xC3", "xxxxxxxxxxxxxxxxxxxxx");
	TrampolineHook(GetEngineBaseHook, (void *)addr, (void **)&GetEngineBaseOriginal);

	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\x81\xEC\x8C\x00\x00\x00\x53\x56\x8B\xF1\x8B\x8E\xCC\x01\x00\x00\x8B\x81\xD0\x02\x00\x00\x8B\x90\xF8\x04\x00\x00\x05\xF8\x04\x00\x00\x89\x54\x24\x08\x8B\x50\x04\x89\x54\x24\x0C\x8B\x40\x08\x89\x44\x24\x10\x8B\x81\xD0\x02\x00\x00\x8B\x88\x04\x05\x00\x00\x05\x04\x05\x00\x00\x89\x4C\x24\x6C\x8B\x50\x04\x57\x8D\x4C\x24\x64\x89\x54\x24\x74\x8B\x40\x08\x51\x8D\x4C\x24\x74\x89\x44\x24\x7C", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	TrampolineHook(UpdateEngineHook, (void *)addr, (void **)&UpdateEngineOriginal);

	// addr = 0x11C6A70;
	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\x33\xC4\x50\x8D\x84\x24\xD8\x00\x00\x00\x64\xA3\x00\x00\x00\x00\x8B\xE9\x89\x6C\x24\x30\x33\xFF\x89\x7C\x24\x4C\x89\x7C\x24\x50\x89\x7C\x24\x54\x89\xBC\x24\xE0\x00\x00\x00\x89\xBC\x24\x9C\x00\x00\x00\x89\xBC\x24\xA0\x00\x00\x00\x89\xBC\x24\xA4\x00\x00\x00", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") - 0x1D;
	LevelLoadBase = addr;
	TrampolineHook(LevelLoadHook, (void *)addr, (void **)&LevelLoadOriginal);

	/* addr = 0x11C6B8E;
	TrampolineHook(LevelLoadHook, (void *)addr, (void **)&LevelLoadOriginal);

	addr = 0x11C7F06;
	SetJMP(LevelLoadFinish, (void *)addr, 0); */

	addr = 0x40AEB0;
	TrampolineHook(DebugPrintHook, (void *)addr, (void **)&DebugPrintOriginal);

	TrampolineHook(LoadLibraryAHook, LoadLibraryA, (void **)&LoadLibraryAOriginal);
}