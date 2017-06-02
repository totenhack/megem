#include "stdafx.h"

DWORD fov_base = 0, dof_base = 0;
void(*GetFOVOriginal)();
void(*GetDOFOriginal)();

__declspec(naked) void GetFOVHook() {
	__asm {
		mov fov_base, edx
		push eax
	}

	fov_base += 0x1D0;

	__asm {
		pop eax
		jmp GetFOVOriginal
	}
}

__declspec(naked) void GetDOFHook() {
	__asm {
		mov dof_base, edi
		push eax
		push ecx
	}

	GetData()->dof_base = dof_base;

	__asm {
		pop ecx
		pop eax
		jmp GetDOFOriginal
	}
}

void SetFOV(float fov) {
	if (fov_base) {
		WriteFloat(GetCurrentProcess(), (void *)(fov_base + 4), fov);
		WriteInt(GetCurrentProcess(), (void *)(fov_base), 19);
	}
}

float GetFOV() {
	return fov_base ? ReadFloat(GetCurrentProcess(), (void *)(fov_base + 4)) : 0;
}

void SetupVisual() {
	DWORD addr;

	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\xF6\x82\xD0\x01\x00\x00\x02\x0F\x84", "xxxxxxxxx");
	TrampolineHook(GetFOVHook, (void *)addr, (void **)&GetFOVOriginal);

	addr = (DWORD)FindPattern((void *)((DWORD)GetModuleHandle(0)), 0x12800000, "\xF3\x0F\x10\x47\x70\x85\xC0\xF3\x0F\x11\x46\x14\x8D\x48\x1C", "xxxxxxxxxxxxxxx");
	TrampolineHook(GetDOFHook, (void *)addr, (void **)&GetDOFOriginal);
}