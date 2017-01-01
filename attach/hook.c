#include <windows.h>
#define JMP_SIZE 5

typedef struct {
  void *src;
  void *dest;
  void *gate;

  char *original;
  char *data;

  unsigned int size;
} HOOK;

void *NewGate(int size) {
  if (size < 0) {
    return NULL;
  }

  size += JMP_SIZE;

  return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE,
                      PAGE_EXECUTE_READWRITE);
}

int memcpy_protect(void *dest, void *src, unsigned int len) {
  if (!len) {
    return 1;
  }

  DWORD p, p1;
  if (!VirtualProtect(dest, len, PAGE_EXECUTE_READWRITE, &p) ||
      !VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &p1))
  {
    return 0;
  }

  memcpy(dest, src, len);

  return VirtualProtect(dest, len, p, &p) && VirtualProtect(src, len, p1, &p1);
}

int SetJMP(void *src, void *dest, unsigned int nops) {
  DWORD protect;
  if (VirtualProtect(src, JMP_SIZE + nops, PAGE_EXECUTE_READWRITE, &protect)) {
    *(char *)src = 0xE9;
    *(void **)(src + 1) = (void *)(dest - (src + JMP_SIZE));

    int i;
    for (i = 0; i < nops; i++) {
      *(char *)(src + JMP_SIZE + i) = 0x90;
    }

    return VirtualProtect(src, JMP_SIZE + nops, protect, &protect);
  }

  return 0;
}

int StaticTrampolineHook(void *src, void *dest, void *gate, int size) {
  if (size < 0 ) {
    return 1;
  }

  size += JMP_SIZE;

  if (!memcpy_protect(gate, src, size)) {
    return 0;
  }

  return SetJMP(gate + size, src + size, 0) &&
         SetJMP(src, dest, size > JMP_SIZE ? size - JMP_SIZE : 0);
}

HOOK *NewHook(void *src, void *dest, void *gate, unsigned int extra) {
  HOOK *hook = (HOOK *)malloc(sizeof(HOOK));

  if (hook != NULL) {
    hook->src  = src;
    hook->dest = dest;
    hook->size = JMP_SIZE + extra;
    hook->gate = !gate ?
        VirtualAlloc(NULL, hook->size,
            MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)
        : gate;

    // Copy original data
    hook->original = (char *)malloc(hook->size);
    if (hook->original != NULL) {
      memcpy_protect(hook->original, hook->src, hook->size);
    }

    // Create the data that will be applied
    hook->data = (char *)malloc(hook->size);
    if (hook->data != NULL) {
      int i;
      for (i = 0; i < extra; i++) {
        hook->data[i] = 0x90; // Fill in nops
      }

      hook->data[i++] = 0xE9; // jmp opcode
      DWORD addr = dest - (src + hook->size); // relative addr
      memcpy_protect(&hook->data[i], &addr, 4);
    }
  }

  return hook;
}

int TrampolineHook(HOOK *hook) {
  return StaticTrampolineHook(hook->src, hook->dest, hook->gate, hook->size);
}

int ApplyHook(HOOK *hook) {
  return memcpy_protect(hook->src, hook->data, hook->size);
}

int RemoveHook(HOOK *hook) {
  return memcpy_protect(hook->src, hook->original, hook->size);
}

void FreeHook(HOOK *hook) {
  if (hook->original) {
    free(hook->original);
  }

  if (hook->data) {
    free(hook->data);
  }

  if (hook->gate) {
    free(hook->gate);
  }

  free(hook);
}

int bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask) {
    for(;*szMask;++szMask,++pData,++bMask)
        if(*szMask=='x' && *pData!=*bMask )
            return 0;

    return !*szMask;
}

DWORD FindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask) {
    for(DWORD i=0; i < dwLen; i++)
        if( bCompare( (BYTE*)( dwAddress+i ),bMask,szMask) )
            return (DWORD)(dwAddress+i);

    return 0;
}
