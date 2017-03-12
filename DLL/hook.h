#pragma once

#define JMP_SIZE 5

void *NewGate(int size);
int memcpy_protect(void *dest, void *src, unsigned int len);
int SetJMP(void *src, void *dest, unsigned int nops);
int StaticTrampolineHook(void *src, void *dest, void *gate, int size);
DWORD FindPattern(DWORD addr, DWORD len, char *bMask, char *szMask);