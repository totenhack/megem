#include "stdafx.h"

static DOLLY *dolly = NewDolly();

EXPORT void EXPORT_ResetDolly() {
	SetFOV(90);

	if (dolly) {
		StopDolly();
		FreeDolly(dolly);
	}

	dolly = NewDolly();
}

EXPORT void EXPORT_SetDollyDOF(float *dof) {
	dolly->dof = *dof;
	FREE(dof);
}

EXPORT void EXPORT_SetDollyDuration(int *duration) {
	dolly->duration = *duration;
	FREE(duration);
}

EXPORT void EXPORT_AddDollyMarker() {
	AddMarker(dolly);
}

EXPORT void EXPORT_PlayDolly() {
	StopDolly();
	PlayDolly(dolly);
}

EXPORT void EXPORT_EnableFloatIncrease() {
	SetFloatIncrease(true);
}

EXPORT void EXPORT_DisableFloatIncrease() {
	SetFloatIncrease(false);
}

EXPORT void EXPORT_EnableFloatDecrease() {
	SetFloatDecrease(true);
}

EXPORT void EXPORT_DisableFloatDecrease() {
	SetFloatDecrease(false);
}

EXPORT void EXPORT_Strang() {
	Strang(true);
}

EXPORT void EXPORT_StopStrang() {
	Strang(false);
}

EXPORT void EXPORT_KickGlitch() {
	KickGlitch(true);
}

EXPORT void EXPORT_StopKickGlitch() {
	KickGlitch(false);
}

EXPORT void EXPORT_EnableGodMode() {
	ToggleGodMode(true);
}

EXPORT void EXPORT_DisableGodMode() {
	ToggleGodMode(false);
}

EXPORT void EXPORT_EnableFloatMode() {
	ToggleFloatMode(true);
}

EXPORT void EXPORT_DisableFloatMode() {
	ToggleFloatMode(false);
}

EXPORT void EXPORT_GetSublevelStatus(char *name) {
	if (name) {
		*name = GetSublevelStatus(name);
	}
}

EXPORT void EXPORT_GetSublevels() {
	GetSublevels();
}

EXPORT void EXPORT_SetSpeed(float *speed_ptr) {
	SetSpeed(*speed_ptr);
	FREE(speed_ptr);
}

EXPORT void EXPORT_SetFOV(float *fov_ptr) {
	SetFOV(*fov_ptr);
	FREE(fov_ptr);
}

EXPORT void EXPORT_SetDOF(float *dof_ptr) {
	SetDOF(*dof_ptr);
	FREE(dof_ptr);
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