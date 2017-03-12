#include "stdafx.h"

bool rendering_enabled = true;

void(*RenderOriginal)();
HRESULT(__stdcall *PresentOriginal)(IDirect3DDevice9 *pDevice, RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);

__declspec(naked) void RenderHook() {
	__asm {
		mov PresentOriginal, edx
		mov edx, PresentHook
		jmp RenderOriginal
	}
}

HRESULT __stdcall PresentHook(IDirect3DDevice9 *pDevice, RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
	DATA *data = GetData();

	data->process_speed = GetSpeed();
	
	return rendering_enabled ? PresentOriginal(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion) : D3D_OK;
}

void DisableRendering() {
	rendering_enabled = false;
}

void EnableRendering() {
	rendering_enabled = true;
}

void SetupRender() {
	// int addr = 0x18B9D93;
	int addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\x8D\x4C\x24\x18\x51\x50\xFF\xD2\x3D\x68\x08\x76\x88\x75\x07", "xxxxxxxxxxxxxxx");
	RenderOriginal = (void(*)())NewGate(0);
	StaticTrampolineHook((void *)addr, RenderHook, RenderOriginal, 0);
}
