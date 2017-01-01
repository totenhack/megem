#include <windows.h>

#define JMP_SIZE 5

typedef struct {
  void *target;
  void *dest;
  void *copy;

  unsigned int size;

  char *original;
  char *data;
} HOOK;

int fill_nops(void *dest, unsigned int count) {
  if (count <= 0) {
    return 1;
  }

  DWORD p;
  if (!VirtualProtect(dest, count, PAGE_EXECUTE_READWRITE, &p)) {
    return 0;
  }

  int i;
  for (i = 0; i < count; i++) {
    *(char *)(dest + i) = 0x90;
  }

  return VirtualProtect(dest, count, p, &p);
}

int memcpy_protect(void *dest, void *src, unsigned int len) {
  if (len <= 0) {
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

HOOK *newHook(void *target, void *dest, unsigned int extra) {
  HOOK *hook = (HOOK *)malloc(sizeof(HOOK));

  if (hook != NULL) {
    unsigned int size = JMP_SIZE + extra;
    DWORD addr;

    hook->target = target;
    hook->dest = dest;

    hook->size = size;

    hook->original = (char *)malloc(size);
    hook->data = (char *)malloc(size);

    // Copy original bytes
    memcpy_protect(hook->original, target, size);

    // JMP data to overwrite original
    hook->data[0] = 0xE9;
    addr = (DWORD)dest - ((DWORD)target + size);
    memcpy_protect(&hook->data[1], &addr, 4);
    fill_nops(&hook->data[JMP_SIZE], size - JMP_SIZE);

    // Create a copied function that is callable
    hook->copy = VirtualAlloc(NULL, size + JMP_SIZE, MEM_COMMIT | MEM_RESERVE,
                              PAGE_EXECUTE_READWRITE);
    memcpy_protect(hook->copy, target, size);
    char bytes[JMP_SIZE] = { 0xE9, 0x90, 0x90, 0x90, 0x90 };
    addr = (DWORD)target - (DWORD)(hook->copy + size);
    memcpy_protect(&bytes[1], &addr, 4);

    memcpy_protect(hook->copy + size, bytes, JMP_SIZE);
  }

  return hook;
}

int applyHook(HOOK *hook) {
  memcpy_protect(hook->target, hook->data, hook->size);
}

int removeHook(HOOK *hook) {
  memcpy_protect(hook->target, hook->original, hook->size);
}

void freeHook(HOOK *hook) {
  removeHook(hook);

  if (hook->original != NULL) {
    free(hook->original);
  }

  if (hook->data != NULL) {
    free(hook->data);
  }

  if (hook->copy != NULL) {
    VirtualFree(hook->copy, 0, MEM_RELEASE);
  }

  free(hook);
}
