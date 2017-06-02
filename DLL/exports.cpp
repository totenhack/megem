#include "stdafx.h"

EXPORT void EXPORT_GetDataBase(DWORD *base) {
	*base = (DWORD)GetData();
}

EXPORT void EXPORT_SetProcessSpeed(float *speed) {
	SetProcessSpeed(*speed);
}

EXPORT void EXPORT_EnableRendering() {
	EnableRendering();
}

EXPORT void EXPORT_DisableRendering() {
	DisableRendering();
}

EXPORT void EXPORT_KG() {
	if (!Loading()) KG(true);
}

EXPORT void EXPORT_Strang() {
	if (!Loading()) Strang(true);
}

EXPORT void EXPORT_Beamer() {
	if (!Loading()) Beamer(true);
}

EXPORT void EXPORT_GodOn() {
	God(true);
}

EXPORT void EXPORT_GodOff() {
	God(false);
}

EXPORT void EXPORT_FloatOn() {
	Float(true);
}

EXPORT void EXPORT_FloatOff() {
	Float(false);
}

EXPORT void EXPORT_OpenDebugConsole() {
	OpenDebugConsole();
}

EXPORT void EXPORT_CloseDebugConsole() {
	CloseDebugConsole();
}

EXPORT void EXPORT_DisableAll() {
	KG(false);
	Beamer(false);
	Strang(false);
}

EXPORT void EXPORT_GetSublevels() {
	GetSublevels();
}

EXPORT void EXPORT_GetSublevelStatus(char *str) {
	if (str) {
		*str = GetSublevelStatus(str);
	}
}

EXPORT void EXPORT_LoadLevelStreamByString(char *str) {
	if (str) {
		LevelStream(GetIdByString(str), 1);
	}
}

EXPORT void EXPORT_UnloadLevelStreamByString(char *str) {
	if (str) {
		LevelStream(GetIdByString(str), 0);
	}
}

EXPORT void EXPORT_SetFOV(float *fov) {
	if (fov && !Loading()) {
		SetFOV(*fov);
	}
}

DOLLY dolly = { 0 };
EXPORT void EXPORT_AddDollyMarker(DWORD *frame) {
	if (!dolly.markers.buffer) {
		dolly.markers = ArrayNew(sizeof(MARKER));
	}

	DWORD i;
	for (i = 0; i < dolly.markers.length; ++i) {
		if (((MARKER *)ArrayGet(&dolly.markers, i))->frame >= *frame) {
			break;
		}
	}

	MARKER marker = GetMarker(*frame);
	ArrayPush(&dolly.markers, &marker);

	for (DWORD e = dolly.markers.length - 1; e > i; --e) {
		ArraySet(&dolly.markers, e, ArrayGet(&dolly.markers, e - 1));
	}

	ArraySet(&dolly.markers, i, &marker);
}

EXPORT void EXPORT_ResetDolly() {
	dolly.markers.length = 0;
	dolly.animations.length = 0;
}

EXPORT void EXPORT_PlayDolly() {
	if (dolly.markers.length < 2) {
		return;
	}

	PlayDolly(&dolly);
}