#define BUFSIZE 512
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0300

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <float.h>
#include <windows.h>
#include <tlhelp32.h>
#include <winuser.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <Objbase.h>
#include <CommCtrl.h>

#define WIDTH 400
#define HEIGHT 450

typedef struct {
  float x, y, z, vx, vy, vz, fy;
  int rx, ry, state, health;
} STATE;

struct {
  int attached, godmode_toggle, floatmode_toggle;
  HANDLE handle;
  DWORD id;
  HANDLE pipe;
  HHOOK keyboard_hook;
  STATE last_ground, before_wallrun, float_state, current;
  HDC hdc;
  HWND hWnd;
} process;

static char keys[256];
static int keybinds[256];
short FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN;

#include "util.c"
#include "process.c"
#include "pipe.c"
#include "game.c"
#include "window.c"

void setup();
void setupKeybinds();

int main(int argc, char *argv[]) {
  // FreeConsole();

  if (getProcessIdByName("megem.exe") != GetCurrentProcessId()) {
    return 1;
  }

  setupKeybinds();

  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)windowMain, NULL, 0, NULL);

  process.attached = process.godmode_toggle = 0;

  for (;;) {
    process.id = getGameProcessId();

    if (process.id != -1) {
      setup();
    } else {
      process.attached = 0;
    }

    Sleep(5000);
  }
}

void update() {
  saveState(&process.current);

  if (process.current.state != WALLRUN) {
    copyState(&process.current, &process.before_wallrun);
  }

  if (process.current.vy == 0 && process.current.state == GROUNDED) {
    copyState(&process.current, &process.last_ground);
  }

  if (process.godmode_toggle) {
    setGameValue(D_HEALTH, 100);
    setGameValue(D_FALL_Y, floatToInt(-2147483647));
  }

  if (process.floatmode_toggle) {
    float s = 30;
    float rx = gameRotToDeg(process.current.rx);

    process.float_state.vx = process.float_state.vz = 0;

    if (keys[FORWARD]) {
      process.float_state.vx = cos(rx * PI / 180) * s;
      process.float_state.vz = sin(rx * PI / 180) * s;
    } else if (keys[BACKWARD]) {
      process.float_state.vx = -cos(rx * PI / 180) * s;
      process.float_state.vz = -sin(rx * PI / 180) * s;
    }

    if (keys[LEFT]) {
      process.float_state.vx += cos((rx - 90) * PI / 180) * s;
      process.float_state.vz += sin((rx - 90) * PI / 180) * s;
    } else if (keys[RIGHT]) {
      process.float_state.vx += cos((rx + 90) * PI / 180) * s;
      process.float_state.vz += sin((rx + 90) * PI / 180) * s;
    }

    if (keys[UP]) {
      process.float_state.vy = s;
    } else if (keys[DOWN]) {
      process.float_state.vy = -s;
    } else {
      process.float_state.vy = 0;
    }

    setVelocity(0, 0, 0);
    setGameValue(D_STATE, GROUNDED - 1);
    setGameValue(D_FALL_Y, floatToInt(-2147483647));
    SetTextColor(process.hdc, RGB(255, 0, 0));
  }

  RECT r = { 0, 0, WIDTH, HEIGHT };
  FillRect(process.hdc, &r, (HBRUSH)(COLOR_WINDOW + 1));

  drawText(process.hdc, 0, 0, " x %f", process.current.x);
  drawText(process.hdc, 0, 20, " y %f", process.current.y);
  drawText(process.hdc, 0, 40, " z %f", process.current.z);

  drawText(process.hdc, 0, 80, " vx %f", process.current.vx);
  drawText(process.hdc, 0, 100, " vy %f", process.current.vy);
  drawText(process.hdc, 0, 120, " vz %f", process.current.vz);

  SetTextColor(process.hdc, RGB(0, 0, 0));

  drawText(process.hdc, 0, 160, " hv %f",
           vToKmh(getVectorDist(process.current.vx, process.current.vz)));
  drawText(process.hdc, 0, 180, " vv %f", vToKmh(process.current.vy));

  drawText(process.hdc, 0, 220, " rx %f",
           gameRotToDeg(process.current.rx));
  drawText(process.hdc, 0, 240, " ry %f",
           gameRotToDeg(process.current.ry));

  if (process.godmode_toggle) {
    SetTextColor(process.hdc, RGB(255, 0, 0));
  }

  drawText(process.hdc, 0, 280, " fy %f", process.current.fy);
  drawText(process.hdc, 0, 300, " h %d", process.current.health);

  SetTextColor(process.hdc, RGB(0, 0, 0));

  drawText(process.hdc, 0, 340, " gs %f",
           intToFloat(getGameValue(D_GAME_SPEED)));

  drawText(process.hdc, 0, 380, " s %d",
	   process.current.state < 10000 ? 0 : process.current.state - 100860160);
}

void setupKeybinds() {
  memset(&keybinds, -1, 256 * sizeof(int));

  FILE *file = fopen("keybinds.dat", "r");

  char buffer[BUFSIZE];
  int t;
  while (fscanf(file, "%s 0x%02x\n", &buffer, &t) >= 0) {
    int i;
    for (i = 0; i < M_COUNT + 5; i++) {
      if (strcmp(buffer, actions[i]) == 0) {
        break;
      }
    }

    keybinds[t] = i;

    if (i >= M_COUNT) {
      switch (M_COUNT + 6 - i) {
        case 6:
          FORWARD = t;
          break;
        case 5:
          BACKWARD = t;
          break;
        case 4:
          LEFT = t;
          break;
        case 3:
          RIGHT = t;
          break;
        case 2:
          UP = t;
          break;
        case 1:
          DOWN = t;
      }
    }
  }

  fclose(file);
}

LRESULT CALLBACK keyboardHook(int code, WPARAM wParam, LPARAM lParam) {
  if (code >= 0 && isGameWindow(GetForegroundWindow())) {
    KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT *)lParam);
    short keycode = data.vkCode;

    if (wParam == WM_KEYDOWN && !keys[keycode]) {
      keys[keycode] = 1;

      SendMessage(process.hWnd, WM_COMMAND, keybinds[keycode], 0);
    } else if (wParam == WM_KEYUP && keys[keycode]) {
      keys[keycode] = 0;
    }
  }

  return CallNextHookEx(NULL, code, wParam, lParam);
}

void setup() {
  setupKeybinds();

  CloseHandle(process.handle);
  process.handle = OpenProcess(PROCESS_ALL_ACCESS, 0, process.id);

  if (!process.handle) {
    printf("Failed to attach to the process\n");
    process.attached = 0;

    return;
  }

  if (!initPipe() && !processHasModule(process.id, "megem.dll")) {
    process.attached = 0;

    if (!injectDll("megem.dll"))  {
      printf("Failed to inject dll\n");
      return;
    }
  }

  if (!process.attached) {
    process.attached = 1;
    printf("Attached Successfully\n");
  }
}
