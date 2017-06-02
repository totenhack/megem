#include "stdafx.h"

SPLINES splines = { 0 };
DWORD frame = 0, length = 0;

MARKER GetMarker(DWORD frame) {
	MARKER marker;
	marker.x = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_X)) / 100;
	marker.y = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Y)) / 100;
	marker.z = ReadFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Z)) / 100;
	marker.rx = IntToDegrees(ReadInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RX)));
	if (marker.rx > 180) marker.rx -= 360;
	marker.ry = (float)(ReadInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RY)) % 0x10000);
	marker.rz = (float)(ReadInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RZ)) % 0x10000);
	marker.frame = frame;
	return marker;
}

void PlayDolly(DOLLY *dolly) {
	StopDolly();

	ARRAY *markers = &dolly->markers;
	length = ((MARKER *)ArrayGet(markers, markers->length - 1))->frame;

	double *sets = (double *)malloc((sizeof(double) * 2) * markers->length);

	for (unsigned int e = 0; e < markers->length; ++e) {
		sets[e * 2] = ((MARKER *)ArrayGet(markers, e))->frame;
	}

	SPLINES s = { 0 };
	for (int i = 0; i < sizeof(SPLINES) / sizeof(SPLINE); ++i) {
		for (unsigned int e = 0; e < markers->length; ++e) {
			sets[(e * 2) + 1] = *(float *)((DWORD)ArrayGet(markers, e) + (i * sizeof(float)));
		}

		SPLINE spline = spline_create(markers->length, sets);
		switch (i) {
			case 0:
				s.x = spline;
				break;
			case 1:
				s.y = spline;
				break;
			case 2:
				s.z = spline;
				break;
			case 3:
				s.rx = spline;
				break;
			case 4:
				s.ry = spline;
				break;
			case 5:
				s.rz = spline;
				break;
		}
	}

	splines = s;
}

void StopDolly() {
	frame = length = 0;

	if (splines.x.equation) {
		for (int i = 0; i < sizeof(splines) / sizeof(SPLINE); ++i) {
			SPLINE *spline = &((SPLINE *)&splines)[i];
			if (spline->equation) {
				free(spline->equation);
			}
			memset(spline, 0, sizeof(SPLINE));
		}
	}

	memset(&splines, 0, sizeof(splines));
}

void UpdateDolly() {
	if (splines.x.equation) {
		if (frame < length) {
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_MOVEMENT_STATE), 32);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_WALKING_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_ACTION_STATE), 0);
			WriteChar(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_HAND_STATE), 0);
			WriteBuffer(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_VX), "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", sizeof(float) * 3);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_X), (float)spline_get(&splines.x, frame) * 100);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Y), (float)spline_get(&splines.y, frame) * 100);
			WriteFloat(GetCurrentProcess(), (void *)(GetPlayerBase() + PLAYER_Z), (float)spline_get(&splines.z, frame) * 100);
			WriteInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RX), (int)DegreesToInt((float)spline_get(&splines.rx, frame)));
			WriteInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RY), (int)spline_get(&splines.ry, frame));
			WriteInt(GetCurrentProcess(), (void *)(GetCameraBase() + CAMERA_RZ), (int)spline_get(&splines.rz, frame));

			++frame;
		} else {
			StopDolly();
		}
	}
}