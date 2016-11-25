#include <tlhelp32.h>

enum {
  D_GAME_SPEED = 0,
  D_POSITION_X,
  D_POSITION_Y,
  D_POSITION_Z,
  D_VELOCITY_X,
  D_VELOCITY_Y,
  D_VELOCITY_Z,
  D_ROTATION_X,
  D_ROTATION_Y,
  D_STATE,
  D_FALL_Y,
  D_HEALTH,
  D_PAUSED
};

DWORD DATA[][6] = {
  { 0x1BFBB70, 0x2FC, 0x9C,  0xC2C, 0x00,  0x00  }, // Game Speed
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xE8  }, // Position x
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xF0  }, // Position y
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xEC  }, // Position z
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x100, 0x0   }, // Velocity x
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x108, 0x0   }, // Velocity y
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x104, 0x0   }, // Velocity z
  { 0x1B73F1C, 0xCC,  0x70,  0xF8,  0x00,  0x00  }, // Rotation x
  { 0x1B73F1C, 0xCC,  0x70,  0xF4,  0x00,  0x00  }, // Rotation y
  { 0x1C4C6FC, 0x78C, 0x780, 0x40,  0x2F8, 0x68  }, // State
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x1C0, 0x72C }, // Fall Height
  { 0x1C553D0, 0xCC,  0x1CC, 0x304, 0x2B8, 0x00  }, // Health
  { 0x1BFBB70, 0x2A4, 0x9C,  0xC54, 0x00,  0x00  } // Paused
};

#define GROUNDED 100860161
#define IN_AIR 100860162
#define WALLRUN 100860172

DWORD getModuleBaseAddress(DWORD pid, char *name) {
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
  MODULEENTRY32 entry = { .dwSize = sizeof(MODULEENTRY32) };

  if (Module32First(snapshot, &entry)) {
    do {
      if (strcmp(entry.szModule, name) == 0) {
        CloseHandle(snapshot);
        return (DWORD)entry.modBaseAddr;
      }
    } while (Module32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);
  return 0;
}

DWORD getGameAddr(int v) {
  static DWORD pid;
  static DWORD base;
  static HANDLE process;

  if (!pid) {
    pid = GetCurrentProcessId();
  }

  if (!base) {
    base = getModuleBaseAddress(pid, "MirrorsEdge.exe");
  }

  if (!process) {
    process = GetCurrentProcess();
  }

  DWORD addr, temp;
  ReadProcessMemory(process, (LPCVOID)(base + DATA[v][0]), &temp, 4, 0);

  for (int i = 1; i < 6; i++) {
    if (!DATA[v][i]) {
      break;
    }

    addr = temp + DATA[v][i];
    ReadProcessMemory(process, (LPCVOID)addr, &temp, 4, 0);
  }

  return addr;
}

DWORD getGameValue(int v) {
  static HANDLE process;

  if (!process) {
    process = GetCurrentProcess();
  }

  DWORD val;
  ReadProcessMemory(process, (LPCVOID)getGameAddr(v), &val, 4, 0);
  return val;
}

HWND getGameWindow() {
  return FindWindowEx(0, 0, "LaunchUnrealUWindowsClient", 0);
}
