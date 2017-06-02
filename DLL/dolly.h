#pragma once

typedef struct {
	float x, y, z, rx, ry, rz;
	float fov, dof;
	DWORD frame;
} MARKER;

typedef struct {
	SPLINE x, y, z, rx, ry, rz;
} SPLINES;

typedef struct {
	ARRAY markers;
	ARRAY animations;
} DOLLY;

MARKER GetMarker(DWORD frame);
void PlayDolly(DOLLY *dolly);
void StopDolly();
void UpdateDolly();