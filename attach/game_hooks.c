void getStringById(int, char *);
int (__fastcall *FrameFunc)(int, float);
int (*LevelStream)(void *);
int (__thiscall *UpdateCheckpoint)(int, int, char **);

HOOK *hook_FrameFunc;

#include <d3d9.h>
#include <d3dx9.h>

LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 *d3ddev;

int __thiscall FrameFuncHook(int this, float frametime) {
  RemoveHook(hook_FrameFunc);
  int ret = FrameFunc(this, frametime);
  ApplyHook(hook_FrameFunc);
  loop();
  return ret;
}

int __fastcall LevelStreamHook(void *info) {
  /* void *eax_copy;
  __asm("movl %%eax, %0" : "=r"(eax_copy)); // save eax register

  MessageBox(0, "Loading", "Loading", 0);

  __asm("movl %0, %%eax" :: "r"(eax_copy)); // restore eax register */
  /* void *ecx;
  __asm("movl %%ecx, %0" : "=r"(ecx)); */
  int ret = LevelStream(info);

  char loading = *(*(char **)(info + 0x88) + 0xC);

  if (loading) {
    MessageBox(0, "loading", "loading", 0);
  } else {
    MessageBox(0, "unloading", "unloading", 0);
  }

  int sublevel_count = *(int *)(info + 0xF4);
  for (int i = 0; i < sublevel_count; i++) {
    char *ptr = *(char **)(info + 0xF4);
    ptr += (i * 12);

    int sublevel_id = *(int *)(ptr + 4);

    char a[255];
    getStringById(sublevel_id, a);
    MessageBox(0, a, a, 0);
  }

  return ret;
}

void getStringById(int id, char *out) {
  if (!id) {
    strcpy(out, "");
    return;
  }

  DWORD base = GetModuleHandle(NULL);
  void *ptr = *(void **)((void *)base + 0x1C4E7D8);
  ptr = *(void **)(ptr + (id * 4));
  ptr += 0x10;

  strcpy(out, ptr);
}

int __thiscall UpdateCheckpointHook(int this, int setting, char **value) {
  if (setting == 900 || setting == 901) {
    char str[255];
    wcstombs(str, (wchar_t *)*value, 254);
    MessageBox(0, str, str, 0);
  }

  return UpdateCheckpoint(this, setting, value);

  /* if (setting == 900 || setting == 901) {
    MessageBox(0, *value, *value, 0);
  } */
}

void installHooks() {
  DWORD base = GetModuleHandle(NULL);

  /* FrameFunc = NewGate(0);
  StaticTrampolineHook((void *)base + 0xE679D0, &FrameFuncHook, FrameFunc, 0); */
  FrameFunc = (void *)base + 0xE679D0;
  hook_FrameFunc = NewHook(FrameFunc, &FrameFuncHook, NULL, 0);
  ApplyHook(hook_FrameFunc); // do a reapply hook because of livesplit crash

  // LevelStream = NewGate(2);
  // StaticTrampolineHook((void *)base + 0xA0F190, &LevelStreamHook, LevelStream, 2);
}
