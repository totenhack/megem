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

DWORD getGameAddr(int v) {
  DWORD base = getModuleBaseAddress(process.id, "MirrorsEdge.exe");
  DWORD addr, temp;
  ReadProcessMemory(process.handle, (LPCVOID)(base + DATA[v][0]), &temp, 4, 0);

  for (int i = 1; i < 6; i++) {
    if (!DATA[v][i]) {
      break;
    }

    addr = temp + DATA[v][i];
    ReadProcessMemory(process.handle, (LPCVOID)addr, &temp, 4, 0);
  }

  return addr;
}

DWORD getGameValue(int v) {
  if (!process.handle || !process.attached) {
    return 0;
  }

  DWORD val;
  ReadProcessMemory(process.handle, (LPCVOID)getGameAddr(v), &val, 4, 0);
  return abs(val) > 2147483647 ? 0 : val;
}

void setGameValue(int v, DWORD val) {
  if (!process.handle || !process.attached) {
    return;
  }

  WriteProcessMemory(process.handle, (LPVOID)getGameAddr(v), &val, 4, 0);
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

void setProcessSpeed(float speed) {
  sendPipeMessage("s%f", speed);
}

void pauseProcess(int y) {
  static float oldSpeed = 1;
  static int prev = 1;

  if (y && prev) {
    prev = 0;

    char buffer[BUFSIZE];
    getPipeValue("speed", buffer);
    oldSpeed = atof(buffer);
    setProcessSpeed(0);
  } else {
    prev = 1;

    setProcessSpeed(oldSpeed);
  }
}

DWORD getGameProcessId() {
  return getProcessIdByName("MirrorsEdge.exe");
}

int isGameRunning() {
  return getGameProcessId() != -1;
}

HWND getGameWindow() {
  return FindWindowEx(0, 0, "LaunchUnrealUWindowsClient", 0);
}

int focusGame() {
  HWND hWnd = getGameWindow();
  return hWnd && SetForegroundWindow(hWnd) && SetActiveWindow(hWnd) &&
         SetFocus(hWnd);
}

float vToKmh(float v) {
  return v * 0.036000;
}

float kmhToV(float kmh) {
  return kmh / 0.036000;
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

void copyState(STATE *src, STATE *dest) {
  memcpy(dest, src, sizeof(STATE));
}

void saveState(STATE *state) {
  if (!process.handle || !process.attached) {
    memset(state, 0, sizeof(STATE));
    return;
  }

  state->x = intToFloat(getGameValue(D_POSITION_X));
  state->y = intToFloat(getGameValue(D_POSITION_Y));
  state->z = intToFloat(getGameValue(D_POSITION_Z));

  state->vx = intToFloat(getGameValue(D_VELOCITY_X));
  state->vy = intToFloat(getGameValue(D_VELOCITY_Y));
  state->vz = intToFloat(getGameValue(D_VELOCITY_Z));

  state->rx = getGameValue(D_ROTATION_X);
  state->ry = getGameValue(D_ROTATION_Y);
  state->state = getGameValue(D_STATE);

  state->fy = intToFloat(getGameValue(D_FALL_Y));
  state->health = getGameValue(D_HEALTH);
}

void loadState(STATE *state) {
  setGameValue(D_STATE, state->state);
  setPosition(state->x, state->y, state->z);
  setVelocity(state->vx, state->vy, state->vz);
  setRotation(state->rx, state->ry);
  setGameValue(D_FALL_Y, floatToInt(state->fy));
  setGameValue(D_HEALTH, state->health);
}

void appendStateToFile(FILE *file, STATE *state) {
  fprintf(file, "%f %f %f %f %f %f %d %d %d %f %d\n",
          state->x, state->y, state->z, state->vx, state->vy, state->vz,
          state->rx, state->ry, state->state, state->fy, state->health);
}

void getStateFromFile(FILE *file, STATE *state) {
  fscanf(file, "%f %f %f %f %f %f %d %d %d %f %d\n",
         &state->x, &state->y, &state->z, &state->vx, &state->vy, &state->vz,
         &state->rx, &state->ry, &state->state, &state->fy, &state->health);
}

void saveStateFile(char *path) {
  STATE state;
  saveState(&state);

  FILE *file = fopen(path, "w");
  appendStateToFile(file, &state);

  DWORD currentState = getGameValue(D_STATE);
  if (currentState == WALLRUN) {
    appendStateToFile(file, &process.last_ground);
    appendStateToFile(file, &process.before_wallrun);
  } else if (currentState == IN_AIR) {
    appendStateToFile(file, &process.last_ground);
  }

  fclose(file);
}

void loadStateFile(char *output) {
  char path[MAX_PATH]; // Save the state in the temp file
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, "megem_temp.medat\0");

  CopyFile(output, path, 0);

  sendPipeMessage("l");

  /* if (access(path, F_OK) == -1) {
    return;
  }

  STATE state, ground, before;
  FILE *file = fopen(path, "r");

  getStateFromFile(file, &state);
  getStateFromFile(file, &ground);
  getStateFromFile(file, &before);

  fclose(file);
  free(path);

  if (state.state == WALLRUN &&
      (getGameValue(D_STATE) == IN_AIR || getGameValue(D_STATE) == GROUNDED))
  {
    unsigned int start = timeGetTime();

    ground.state = IN_AIR;
    ground.vy = ground.vx = ground.vz = 0;
    ground.fy = -2147483647;
    loadState(&ground);

    while (getGameValue(D_STATE) != GROUNDED) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }

    SendMessage(getGameWindow(), WM_KEYDOWN, VK_SPACE, 0);
    SendMessage(getGameWindow(), WM_KEYUP, VK_SPACE, 0);

    while (getGameValue(D_STATE) == GROUNDED) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }

    loadState(&before);

    /* before.x -= before.vx * 0.1;
    before.y += 10;
    before.z -= before.vz * 0.1;
    before.state = IN_AIR;

    while (getGameValue(D_STATE) != WALLRUN) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }

    setGameValue(D_FALL_Y, -2147483647);
    setRotation(before.rx, before.ry);
    setPosition(state.x, state.y, state.z); */

    /* ground.state = IN_AIR;
    ground.vy = ground.vx = ground.vz = 0;
    ground.fy = -2147483647;
    loadState(&ground);

    while (getGameValue(D_STATE) != GROUNDED) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }

    ground.rx = before.rx;
    ground.ry = before.ry;
    ground.vx = before.vx;
    ground.vz = before.vz;
    ground.state = GROUNDED;
    loadState(&ground);

    SendMessage(getGameWindow(), WM_KEYDOWN, VK_SPACE, 0);
    SendMessage(getGameWindow(), WM_KEYUP, VK_SPACE, 0);

    while (getGameValue(D_STATE) == GROUNDED) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }

    setGameValue(D_FALL_Y, -2147483647);
    setRotation(before.rx, before.ry);
    setPosition(before.x, before.y, before.z);

    while (getGameValue(D_STATE) != WALLRUN) {
      if (timeGetTime() > start + 250) {
        return loadState(&state);
      }

      Sleep(1);
    }
  } else if (getGameValue(D_STATE) != state.state) {
    state.state = IN_AIR;
  }

  loadState(&state); */
}

void saveStateSlot(int slot) {
  char path[MAX_PATH];
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, "megem_save_\0");
  sprintf(path + strlen(path), "%d.medat", slot);

  saveStateFile(path);
}

void loadStateSlot(int slot) {
  char *path = (char *)malloc(MAX_PATH);
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, "megem_save_\0");
  sprintf(path + strlen(path), "%d.medat", slot);

  loadStateFile(path);
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

int suspendGame() {
  if (!isGameRunning()) {
    return 0;
  }

  THREADENTRY32 entry = { .dwSize = sizeof(THREADENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  DWORD pid = getGameProcessId();

  // Get the thread id of the injected dll -- only thread we don't to stop
  char buffer[255];
  getPipeValue("pid", buffer);
  DWORD dllId = atof(buffer);

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
  if (!isGameRunning()) {
    return 0;
  }

  THREADENTRY32 entry = { .dwSize = sizeof(THREADENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  DWORD pid = getGameProcessId();

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

int getGameThreadCount() {
  THREADENTRY32 entry = { .dwSize = sizeof(THREADENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  DWORD pid = getGameProcessId();
  int count = 0;

  if (Thread32First(snapshot, &entry)) {
    do {
      if (entry.th32OwnerProcessID == pid) {
        count++;
      }
    } while(Thread32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);

  return count;
}

float getVectorDist(float vx, float vz) {
  return sqrt(vx * vx + vz * vz);
}
