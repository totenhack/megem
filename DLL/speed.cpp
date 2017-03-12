#include "stdafx.h"

float speed = 1;

LONGLONG QueryPerformanceCounterInitial, QueryPerformanceCounterOffset;
DWORD GetTickCountInitial, GetTickCountOffset;
DWORD timeGetTimeInitial, timeGetTimeOffset;

BOOL(WINAPI *QueryPerformanceCounterOriginal)(LARGE_INTEGER *lpPerformanceCount);
DWORD(WINAPI *GetTickCountOriginal)();
DWORD(__stdcall *timeGetTimeOriginal)();

BOOL WINAPI QueryPerformanceCounterHook(LARGE_INTEGER *lpPerformanceCount) {
	LARGE_INTEGER current;

	BOOL ret = QueryPerformanceCounterOriginal(&current);

	lpPerformanceCount->QuadPart = (LONGLONG)((current.QuadPart - QueryPerformanceCounterInitial) * speed) + QueryPerformanceCounterOffset;

	return ret;
}

DWORD WINAPI GetTickCountHook() {
	return (DWORD)((GetTickCountOriginal() - GetTickCountInitial) * speed) + GetTickCountOffset;
}

DWORD __stdcall timeGetTimeHook() {
	return (DWORD)((timeGetTimeOriginal() - timeGetTimeInitial) * speed) + timeGetTimeOffset;
}

void SetSpeed(float s) {
	GetTickCountOffset = GetTickCount();
	GetTickCountInitial = GetTickCountOriginal();

	timeGetTimeOffset = timeGetTime();
	timeGetTimeInitial = timeGetTimeOriginal();

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	QueryPerformanceCounterOffset = t.QuadPart;
	QueryPerformanceCounterOriginal(&t);
	QueryPerformanceCounterInitial = t.QuadPart;

	speed = s;
	GetData()->process_speed = speed;
}

float GetSpeed() {
	return speed;
}

void SetupSpeed() {
	QueryPerformanceCounterOriginal = (BOOL(WINAPI *)(LARGE_INTEGER *))NewGate(0);
	StaticTrampolineHook(QueryPerformanceCounter, QueryPerformanceCounterHook, QueryPerformanceCounterOriginal, 0);

	GetTickCountOriginal = (DWORD(WINAPI *)())NewGate(2);
	StaticTrampolineHook(GetTickCount, GetTickCountHook, GetTickCountOriginal, 2);

	timeGetTimeOriginal = (DWORD(__stdcall *)())NewGate(1);
	StaticTrampolineHook(timeGetTime, timeGetTimeHook, timeGetTimeOriginal, 1);

	SetSpeed(1);
}