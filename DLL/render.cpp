#include "stdafx.h"

bool rendering_enabled = true;

HRESULT(__stdcall *PresentOriginal)(IDirect3DDevice9 *pDevice, RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);

void WriteText(LPDIRECT3DDEVICE9 device, int pt, UINT weight, DWORD align, char *font, DWORD color, int x, int y, char *text, int length) {
	LPD3DXFONT lpFont;
	D3DXCreateFontA(device, pt, 0, weight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font, &lpFont);

	RECT r = { x, y, x, y };
	lpFont->DrawTextA(NULL, text, length, &r, DT_NOCLIP | align, color);
	lpFont->Release();
}

#define WATERMARK "Megem V1.0"

HRESULT __stdcall PresentHook(IDirect3DDevice9 *pDevice, RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
	++(GetData()->frame);
	WriteText(pDevice, 25, FW_NORMAL, DT_LEFT, "Arial", D3DCOLOR_ARGB(255, 255, 0, 0), 5, 5, WATERMARK, strlen(WATERMARK));
	return rendering_enabled ? PresentOriginal(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion) : D3D_OK;
}

void DisableRendering() {
	rendering_enabled = false;
}

void EnableRendering() {
	rendering_enabled = true;
}

void SetupRender() {
	TrampolineHook(PresentHook, (void *)GetD3D9Exports()[D3D9_EXPORT_PRESENT], (void **)&PresentOriginal);
}