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
  { 0x1BFBB70, 0x1C,  0x9C,  0xC2C, 0x00,  0x00  }, // Game Speed
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xE8  }, // Position x
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xF0  }, // Position y
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x624, 0xEC  }, // Position z
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x100, 0x0   }, // Velocity x
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x108, 0x0   }, // Velocity y
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x104, 0x0   }, // Velocity z
  { 0x1B73F1C, 0xCC,  0x70,  0xF8,  0x00,  0x00  }, // Rotation x
  { 0x1B73F1C, 0xCC,  0x70,  0xF4,  0x00,  0x00  }, // Rotation y
  { 0x1B73F1C, 0xCC,  0x4A4, 0x214, 0x98,  0x68  }, // State
  { 0x1C553D0, 0xCC,  0x1D4, 0x70,  0x1C0, 0x72C }, // Fall Height
  { 0x1C553D0, 0xCC,  0x1CC, 0x304, 0x2B8, 0x00  }, // Health
  { 0x1BFBB70, 0x3C,  0x9C,  0xC54, 0x00,  0x00  } // Paused
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

void setGameValue(int v, DWORD val) {
  static HANDLE process;

  if (!process) {
    process = GetCurrentProcess();
  }

  WriteProcessMemory(process, (LPVOID)getGameAddr(v), &val, 4, 0);
}

int isGameWindow(HWND hWnd) {
  char title[255];
  char check[] = "Mirror's Edge";
  GetWindowText(hWnd, title, 255);

  int i;
  for (i = 0; i < 13; i++) {
    if (check[i] != title[i]) {
      return 0;
    }
  }

  return 1;
}

HWND getGameWindow() {
  return FindWindowEx(0, 0, "LaunchUnrealUWindowsClient", 0);
}

float gameRotToDeg(int a) {
  float to360 = ((float)a / 65536) * 360;

  float rot = fmod(to360, 360);

  return rot < 0 ? rot + 360 : rot;
}

int degToGameRot(float a) {
  float to65536 = ((a) / 360) * 65536;

  return (int)to65536 % 65536;
}

float vToKmh(float v) {
  return v * 0.036000;
}

float kmhToV(float kmh) {
  return kmh / 0.036000;
}

float getVectorDist(float vx, float vz) {
  return sqrt(vx * vx + vz * vz);
}

void copyState(STATE *src, STATE *dest) {
  memcpy(dest, src, sizeof(STATE));
}

int suspendGame() {
  THREADENTRY32 entry = { .dwSize = sizeof(THREADENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  DWORD pid = GetCurrentProcessId();

  DWORD dllId = GetCurrentThreadId();

  if (Thread32First(snapshot, &entry)) {
    do {
      if (entry.th32OwnerProcessID == pid && entry.th32ThreadID != dllId) {
        HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0,
                                   entry.th32ThreadID);
        SuspendThread(thread);
        CloseHandle(thread);
      }
    } while(Thread32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);

  return 1;
}

int resumeGame() {
  THREADENTRY32 entry = { .dwSize = sizeof(THREADENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  DWORD pid = GetCurrentProcessId();

  // Resume all threads
  if (Thread32First(snapshot, &entry)) {
    do {
      if (entry.th32OwnerProcessID == pid) {
        HANDLE thread = OpenThread(PROCESS_CREATE_THREAD, 0,
                                   entry.th32ThreadID);
        ResumeThread(thread);
        CloseHandle(thread);
      }
    } while(Thread32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);

  return 1;
}

void setPosition(float x, float y, float z) {
  setGameValue(D_POSITION_X, floatToInt(x));
  setGameValue(D_POSITION_Y, floatToInt(y));
  setGameValue(D_POSITION_Z, floatToInt(z));
}

void setVelocity(float vx, float vy, float vz) {
  setGameValue(D_VELOCITY_X, floatToInt(vx));
  setGameValue(D_VELOCITY_Y, floatToInt(vy));
  setGameValue(D_VELOCITY_Z, floatToInt(vz));
}

void setRotation(int x, int y) {
  setGameValue(D_ROTATION_X, x);
  setGameValue(D_ROTATION_Y, y);
}

void loadState(STATE *state) {
  setGameValue(D_STATE, state->state);
  setPosition(state->x, state->y, state->z);
  setVelocity(state->vx, state->vy, state->vz);
  setRotation(state->rx, state->ry);
  setGameValue(D_FALL_Y, floatToInt(state->fy));
  setGameValue(D_HEALTH, state->health);
}

void getStateFromFile(FILE *file, STATE *state) {
  fscanf(file, "%f %f %f %f %f %f %d %d %d %f %d\n",
         &state->x, &state->y, &state->z, &state->vx, &state->vy, &state->vz,
         &state->rx, &state->ry, &state->state, &state->fy, &state->health);
}

void loadTempState() {
  char path[MAX_PATH];
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, "megem_temp.medat\0");

  STATE state, ground, before;
  FILE *file = fopen(path, "r");

  getStateFromFile(file, &state);
  getStateFromFile(file, &ground);
  getStateFromFile(file, &before);

  fclose(file);

  load_state.frame = 0;

  DWORD current = getGameValue(D_STATE);
  if (state.state == WALLRUN && (current == GROUNDED || current == IN_AIR)) {
    ground.state = IN_AIR;
    ground.vy = -250;
    ground.vx = before.vx;
    ground.vz = before.vz;
    ground.rx = before.rx;
    ground.ry = before.ry;
    ground.fy = before.fy = -2147483647;

    before.x -= sign(before.vx) * sqrt(abs(before.vx));
    before.z -= sign(before.vz) * sqrt(abs(before.vz));

    copyState(&state, &load_state.state);
    copyState(&before, &load_state.before);

    load_state.load = 1;
    loadState(&ground);
  } else {
    load_state.load = 0;
    state.state = IN_AIR;
    loadState(&state);
  }
}
