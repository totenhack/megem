int (WINAPI *oQueryPerformanceCounter)(LARGE_INTEGER *);
DWORD (WINAPI *oGetTickCount)();

int WINAPI hQueryPerformanceCounter(LARGE_INTEGER *);
DWORD WINAPI hGetTickCount();

void setSpeed(float);
void loop();

struct {
  LONGLONG initial, offset;
} QPC;

struct {
  DWORD initial, offset;
} GTC;

float speed_multiplier = 1;

void initSpeedMod() {
  HANDLE kernel32 = GetModuleHandleA("kernel32.dll");

  void *qpcAddr = GetProcAddress(kernel32, "QueryPerformanceCounter");
  void *gtcAddr = GetProcAddress(kernel32, "GetTickCount");

  oQueryPerformanceCounter = NewGate(0);
  oGetTickCount = NewGate(4);

  StaticTrampolineHook(qpcAddr, &hQueryPerformanceCounter,
                       oQueryPerformanceCounter, 0);
  StaticTrampolineHook(gtcAddr, &hGetTickCount, oGetTickCount, 4);

  setSpeed(1);
}

void setSpeed(float speed) {
  GTC.offset = GetTickCount();
  GTC.initial = oGetTickCount();

  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  QPC.offset = t.QuadPart;

  oQueryPerformanceCounter(&t);
  QPC.initial = t.QuadPart;

  speed_multiplier = speed;

  setPipeValue("speed", "%f", speed);
}

void advanceFrame() {
  if (speed_multiplier != 0) {
    setSpeed(0);
  }

  LARGE_INTEGER qpf;
  QueryPerformanceFrequency(&qpf);

  QPC.offset += (60 * qpf.QuadPart) / 1000;
  GTC.offset += 60;
}

int WINAPI hQueryPerformanceCounter(LARGE_INTEGER *out) {
  LARGE_INTEGER current;
  int ret = oQueryPerformanceCounter(&current);

  out->QuadPart =
      ((current.QuadPart - QPC.initial) * speed_multiplier) + QPC.offset;

  return ret;
}

DWORD WINAPI hGetTickCount() {
  return ((oGetTickCount() - GTC.initial) * speed_multiplier) + GTC.offset;
}
