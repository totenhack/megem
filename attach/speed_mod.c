HOOK *qpc_hook;
HOOK *gtc_hook;

int (WINAPI *oQPC)(LARGE_INTEGER *);

LARGE_INTEGER qpc_initial, qpc_offset, qpf;

float speed = 1;

int WINAPI hQPC(LARGE_INTEGER *o) {
  LARGE_INTEGER current;
  int ret = oQPC(&current);
  o->QuadPart = qpc_offset.QuadPart +
                ((current.QuadPart - qpc_initial.QuadPart) * speed);

  return ret;
}

DWORD WINAPI hGTC() {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);

  return (current.QuadPart * 1000) / qpf.QuadPart;
}

void setSpeed(float s) {
  QueryPerformanceFrequency(&qpf);

  QueryPerformanceCounter(&qpc_offset);
  oQPC(&qpc_initial);

  speed = s;
  setPipeValue("speed", "%f", speed);
}

void advanceFrame() {
  if (speed != 0) {
    setSpeed(0);
  }

  qpc_offset.QuadPart += (17 * qpf.QuadPart) / 1000;
}

void initSpeedMod() {
  void *qpcAddr = (void *)GetProcAddress(GetModuleHandle("Kernel32.dll"),
                                         "QueryPerformanceCounter");
  qpc_hook = newHook(qpcAddr, hQPC, 0);
  oQPC = qpc_hook->copy;

  void *gtcAddr = (void *)GetProcAddress(GetModuleHandle("Kernel32.dll"),
                                         "GetTickCount");
  gtc_hook = newHook(gtcAddr, hGTC, 0);

  setSpeed(1);

  applyHook(qpc_hook);
  applyHook(gtc_hook);
}
