#pragma once

void SetupRender();
HRESULT __stdcall PresentHook(IDirect3DDevice9 *pDevice, RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);
void DisableRendering();
void EnableRendering();