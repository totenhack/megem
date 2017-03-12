#pragma once

typedef struct {
	float x, y, z, rx, ry, fov;
} MARKER;

typedef struct {
	MARKER *markers;
	int marker_count, markers_alloc;
	int frame, duration;

	float dof;
} DOLLY;

#define EASE(t, a, b) (a + ((b - a) * (t<.5 ? 2*t*t : -1+(4-2*t)*t)))

DOLLY *NewDolly();
void FreeDolly(DOLLY *dolly);
void AddMarker(DOLLY *dolly);
void SetupDolly();
void PlayDolly(DOLLY *dolly);
void StopDolly();
void UpdateDolly();
float GetFOV();
void SaveMarker(MARKER *marker);
void LoadMarker(MARKER *marker);
void SetFOV(float fov);
void SetDOF(float dof);
__declspec(dllexport) void EXPORT_SetFOV(float *fov_ptr);
__declspec(dllexport) void EXPORT_SetDOF(float *dof_ptr);