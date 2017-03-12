#include "stdafx.h"

void *NewGate(int size) {
	if (size < 0) {
		return NULL;
	}

	size += (JMP_SIZE * 2);

	return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

int memcpy_protect(void *dest, void *src, unsigned int len) {
	if (!len) {
		return 1;
	}

	DWORD p, p1;
	if (!VirtualProtect(dest, len, PAGE_EXECUTE_READWRITE, &p) ||
		!VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &p1)) {
		return 0;
	}

	memcpy(dest, src, len);

	return VirtualProtect(dest, len, p, &p) && VirtualProtect(src, len, p1, &p1);
}

int SetJMP(void *src, void *dest, unsigned int nops) {
	DWORD protect;
	if (VirtualProtect(src, JMP_SIZE + nops, PAGE_EXECUTE_READWRITE, &protect)) {
		*(char *)src = (char)0xE9;
		*(void **)((DWORD)src + 1) = (void *)((DWORD)dest - ((DWORD)src + JMP_SIZE));

		for (int i = 0; i < (int)nops; i++) {
			*(char *)((DWORD)src + JMP_SIZE + i) = (char)0x90;
		}

		return VirtualProtect(src, JMP_SIZE + nops, protect, &protect);
	}

	return 0;
}

int StaticTrampolineHook(void *src, void *dest, void *gate, int size) {
	if (size < 0) {
		return 1;
	}

	size += JMP_SIZE;

	if (!memcpy_protect(gate, src, size)) {
		return 0;
	}

	return SetJMP((void *)((DWORD)gate + size), (void *)((DWORD)src + size), 0) && SetJMP(src, dest, size - JMP_SIZE);
}

int bCompare(char *start, char *bMask, char* szMask) {
	for (; *szMask; ++szMask, ++start, ++bMask) {
		if (*szMask == 'x' && *start != *bMask) {
			return 0;
		}
	}

	return !*szMask;
}

DWORD FindPattern(DWORD addr, DWORD len, char *bMask, char *szMask) {
	for (DWORD i = 0; i < len; i++) {
		if (bCompare((char *)(addr + i), bMask, szMask)) {
			return (DWORD)(addr + i);
		}
	}

	return 0;
}