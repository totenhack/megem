#include "stdafx.h"

DOLLY *dolly = NULL;
SPLINE spline_x, spline_y, spline_z, spline_rx, spline_ry;

int fov_base, dof_base;

void(*GetFOVOriginal)();
void(*GetDOFOriginal)();

DOLLY *NewDolly() {
	DOLLY *d = (DOLLY *)malloc(sizeof(DOLLY));
	memset(d, 0, sizeof(DOLLY));

	d->markers_alloc = 0xFF;
	d->markers = (MARKER *)malloc(sizeof(MARKER) * d->markers_alloc);

	d->dof = 0;

	return d;
}

void FreeDolly(DOLLY *d) {
	if (d->markers) {
		free(d->markers);
	}

	free(d);
}

void AddMarker(DOLLY *d) {
	if (d->marker_count >= d->markers_alloc) {
		d->markers_alloc *= 2;
		d->markers = (MARKER *)realloc(d->markers, d->markers_alloc);
	}

	SaveMarker(&d->markers[d->marker_count++]);
}

void PlayDolly(DOLLY *d) {
	double *sets = (double *)malloc((d->marker_count * 2) * sizeof(double));

	for (int i = 0; i < d->marker_count; i++) {
		sets[i * 2] = (d->duration / (d->marker_count - 1)) * i;
		sets[(i * 2) + 1] = d->markers[i].x;
	}

	if (spline_x.equation) free(spline_x.equation);
	spline_x = spline_create(d->marker_count, sets);

	for (int i = 0; i < d->marker_count; i++) {
		sets[i * 2] = (d->duration / (d->marker_count - 1)) * i;
		sets[(i * 2) + 1] = d->markers[i].y;
	}

	if (spline_y.equation) free(spline_y.equation);
	spline_y = spline_create(d->marker_count, sets);

	for (int i = 0; i < d->marker_count; i++) {
		sets[i * 2] = (d->duration / (d->marker_count - 1)) * i;
		sets[(i * 2) + 1] = d->markers[i].z;
	}

	if (spline_z.equation) free(spline_z.equation);
	spline_z = spline_create(d->marker_count, sets);

	for (int i = 0; i < d->marker_count; i++) {
		sets[i * 2] = (d->duration / (d->marker_count - 1)) * i;
		sets[(i * 2) + 1] = d->markers[i].rx;
	}

	if (spline_rx.equation) free(spline_rx.equation);
	spline_rx = spline_create(d->marker_count, sets);

	for (int i = 0; i < d->marker_count; i++) {
		sets[i * 2] = (d->duration / (d->marker_count - 1)) * i;
		sets[(i * 2) + 1] = d->markers[i].ry;
	}

	if (spline_ry.equation) free(spline_ry.equation);
	spline_ry = spline_create(d->marker_count, sets);

	free(sets);

	d->frame = 0;
	dolly = d;
}

void StopDolly() {
	if (dolly) {
		dolly = NULL;
		*(char *)(GetPlayerBase() + PLAYER_STATE) = 2;
		SetFOV(90);
	}
}

void UpdateDolly() {
	if (!dolly || dolly->marker_count <= 0) {
		StopDolly();
		return;
	}

	/* int count = dolly->marker_count - 1;
	count = count < 0 ? 0 : count;

	int index = (int)(((float)dolly->frame / dolly->duration) * count);

	if (count == 0 || dolly->frame >= dolly->duration || index >= count) {
		LoadMarker(&dolly->markers[count]);
		StopDolly();
		return;
	}

	MARKER *marker = &dolly->markers[index];
	MARKER *next_marker = &dolly->markers[index + 1];

	float duration = (float)dolly->duration / count;
	float relative = (float)fmod(dolly->frame, duration) / duration;

	MARKER new_marker;

	new_marker.x = EASE(relative, marker->x, next_marker->x);
	new_marker.y = EASE(relative, marker->y, next_marker->y);
	new_marker.z = EASE(relative, marker->z, next_marker->z);
	new_marker.rx = EASE(relative, marker->rx, next_marker->rx);
	new_marker.ry = EASE(relative, marker->ry, next_marker->ry);
	
	LoadMarker(&new_marker);

	SetFOV(EASE(relative, marker->fov, next_marker->fov));
	SetDOF(dolly->dof); */

	MARKER new_marker;

	new_marker.x = (float)spline_get(&spline_x, dolly->frame);
	new_marker.y = (float)spline_get(&spline_y, dolly->frame);
	new_marker.z = (float)spline_get(&spline_z, dolly->frame);
	new_marker.rx = (float)spline_get(&spline_rx, dolly->frame);
	new_marker.ry = (float)spline_get(&spline_ry, dolly->frame);

	LoadMarker(&new_marker);

	*(char *)(GetPlayerBase() + PLAYER_STATE) = 0;
	*(int *)(GetPlayerBase() + PLAYER_HEALTH) = 100;

	dolly->frame++;
}

void SaveMarker(MARKER *marker) {
	int base = GetPlayerBase();
	marker->x = *(float *)(base + PLAYER_X);
	marker->y = *(float *)(base + PLAYER_Y);
	marker->z = *(float *)(base + PLAYER_Z);
	marker->fov = *(float *)(fov_base + 4);

	base = GetCameraBase();
	marker->rx = IntToDegrees((*(int *)(base + CAMERA_RX) % 0x10000));
	marker->rx -= marker->rx > 180 ? 360 : 0;
	marker->ry = IntToDegrees((*(int *)(base + CAMERA_RY) % 0x10000));
}

void LoadMarker(MARKER *marker) {
	int base = GetPlayerBase();
	*(float *)(base + PLAYER_X) = marker->x;
	*(float *)(base + PLAYER_Y) = marker->y;
	*(float *)(base + PLAYER_Z) = marker->z;

	base = GetCameraBase();
	*(int *)(base + CAMERA_RX) = DegreesToInt(marker->rx);
	*(int *)(base + CAMERA_RY) = DegreesToInt(marker->ry);
}

void SetFOV(float fov) {
	if (fov_base) {
		*(float *)(fov_base + 4) = fov;
		*(int *)(fov_base) = 19;
	}
}

void SetDOF(float dof) {
	if (dof_base) {
		*(float *)(dof_base + 0x70) = dof;
	}
}

float GetFOV() {
	return fov_base ? *(float *)(fov_base + 4) : 0;
}

__declspec(naked) void GetFOVHook() {
	static int temp_eax;
	
	__asm {
		mov fov_base, edx
		mov temp_eax, eax
	}

	fov_base += 0x1D0;

	__asm {
		mov eax, temp_eax
		jmp GetFOVOriginal
	}
}

__declspec(naked) void GetDOFHook() {
	__asm {
		mov dof_base, edi
		jmp GetDOFOriginal
	}
}

void SetupDolly() {
	int addr;

	// addr = 0xB40466;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\xF6\x82\xD0\x01\x00\x00\x02\x0F\x84", "xxxxxxxxx");
	GetFOVOriginal = (void(*)())NewGate(2);
	StaticTrampolineHook((void *)addr, GetFOVHook, GetFOVOriginal, 2);

	// addr = 0xFDB3E2;
	addr = FindPattern((DWORD)GetModuleHandle(0), 0x12800000, "\xF3\x0F\x10\x47\x70\x85\xC0\xF3\x0F\x11\x46\x14\x8D\x48\x1C", "xxxxxxxxxxxxxxx");
	GetDOFOriginal = (void(*)())NewGate(0);
	StaticTrampolineHook((void *)addr, GetDOFHook, GetDOFOriginal, 0);
}