#include "stdafx.h"

float process_speed = 1;
LONGLONG QueryPerformanceCounterInitial, QueryPerformanceCounterOffset;
DWORD GetTickCountInitial, GetTickCountOffset;
DWORD timeGetTimeInitial, timeGetTimeOffset;

BOOL(WINAPI *QueryPerformanceCounterOriginal)(LARGE_INTEGER *lpPerformanceCount);
DWORD(WINAPI *GetTickCountOriginal)();
DWORD(__stdcall *timeGetTimeOriginal)();

BOOL WINAPI QueryPerformanceCounterHook(LARGE_INTEGER *lpPerformanceCount) {
	LARGE_INTEGER current;

	BOOL ret = QueryPerformanceCounterOriginal(&current);

	lpPerformanceCount->QuadPart = (LONGLONG)((current.QuadPart - QueryPerformanceCounterInitial) * process_speed) + QueryPerformanceCounterOffset;

	return ret;
}

DWORD WINAPI GetTickCountHook() {
	return (DWORD)((GetTickCountOriginal() - GetTickCountInitial) * process_speed) + GetTickCountOffset;
}

DWORD __stdcall timeGetTimeHook() {
	return (DWORD)((timeGetTimeOriginal() - timeGetTimeInitial) * process_speed) + timeGetTimeOffset;
}

void SetProcessSpeed(float s) {
	GetTickCountOffset = GetTickCount();
	GetTickCountInitial = GetTickCountOriginal();

	timeGetTimeOffset = timeGetTime();
	timeGetTimeInitial = timeGetTimeOriginal();

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	QueryPerformanceCounterOffset = t.QuadPart;
	QueryPerformanceCounterOriginal(&t);
	QueryPerformanceCounterInitial = t.QuadPart;

	process_speed = s;
	GetData()->process_speed = s;
}

void SetupSpeed() {
	TrampolineHook(QueryPerformanceCounterHook, QueryPerformanceCounter, (void **)&QueryPerformanceCounterOriginal);
	TrampolineHook(GetTickCountHook, GetTickCount, (void **)&GetTickCountOriginal);
	TrampolineHook(timeGetTimeHook, timeGetTime, (void **)&timeGetTimeOriginal);

	SetProcessSpeed(1);
}