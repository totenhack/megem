#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include <math.h>
#include <windows.h>

#define BUFSIZE 512

typedef struct {
  float x, y, z, vx, vy, vz, fy;
  int rx, ry, state, health;
} STATE;

struct {
  int frame, load;
  STATE state, before;
} load_state;

int kg_toggle     = 0,
    beamer_toggle = 0,
    strang_toggle = 0;

#include "util.c"
#include "game.c"
#include "pipe.c"
#include "hook.c"
#include "speed_mod.c"
#include "game_hooks.c"
#include "effects.c"

int WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
void newClient(LPVOID);
void doCommand(char *);
void loop();

struct {
  float x, y, z, vx, vy, vz;
} float_data;

void MainThread(HINSTANCE dll) {
  HANDLE pipe;
  LPTSTR pipeName = "\\\\.\\pipe\\megem";

  int connected;

  for (;;) {
    pipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE |
                           PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_WAIT,
                           PIPE_UNLIMITED_INSTANCES, BUFSIZE, BUFSIZE, 0, NULL);

    connected = ConnectNamedPipe(pipe, NULL) ? 1 :
                    (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected) {
      CreateThread(0, 0, (LPTHREAD_START_ROUTINE)newClient, (LPVOID)pipe, 0, 0);
    }
  }

  CloseHandle(pipe);
}

void newClient(LPVOID lParam) {
  HANDLE pipe = (HANDLE)lParam;
  char bufferIn = (char *)malloc(BUFSIZE);
  DWORD bufferInLength;

  int success;
  for (;;) {
    success = ReadFile(pipe, bufferIn, BUFSIZE, &bufferInLength, NULL);

    if (!success) {
      free(command);
      break;
    } else {
      bufferIn[bufferInLength] = '\0';
      doCommand(bufferIn);
    }
  }

  FlushFileBuffers(pipe);
  DisconnectNamedPipe(pipe);
}

void doCommand(char *command) {
  char c = command[0];
  memmove(command, command + 1, strlen(command));

  switch (c) {
    // Exit process
    case 'e':
      ExitProcess(0);
      break;
    // Process speed
    case 's':
      setSpeed(atof(command));
      break;
    // Advance frame
    case 'a':
      advanceFrame();
      break;
    // Kg
    case 'k': {
      if (!kg_toggle && !getGameValue(D_PAUSED)) {
        kg_toggle = 1;
      }

      break;
    }
    // Init float mode
    case 'f':
      float_data.x = intToFloat(getGameValue(D_POSITION_X));
      float_data.y = intToFloat(getGameValue(D_POSITION_Y));
      float_data.z = intToFloat(getGameValue(D_POSITION_Z));
      break;
    // Load state
    case 'l':
      loadTempState();
      break;

    /*
    // Beamer
    case 'b':
      beamer_toggle = !beamer_toggle;
      break;
    // Strang
    case 't':
      strang_toggle = !strang_toggle;
      break;
    */
  }

  free(command);
}

void loop() {
  // effectsLoop();

  int focused = isGameWindow(GetForegroundWindow());

  if (kg_toggle) {
    DWORD state = getGameValue(D_STATE);

    if (state == WALLRUN) {
      SendMessage(getGameWindow(), WM_LBUTTONDOWN, MK_LBUTTON, 0);
      SendMessage(getGameWindow(), WM_LBUTTONUP, MK_LBUTTON, 0);
    } else if (state == GROUNDED) {
      SendMessage(getGameWindow(), WM_KEYDOWN, VK_SPACE, VK_SPACE);
      SendMessage(getGameWindow(), WM_KEYUP, VK_SPACE, VK_SPACE);
      kg_toggle = 0;
    }
  }

  if (load_state.load) {
    switch (load_state.frame) {
      case 0:
        if (getGameValue(D_STATE) == GROUNDED) {
          SendMessage(getGameWindow(), WM_KEYDOWN, VK_SPACE, 0);
          SendMessage(getGameWindow(), WM_KEYUP, VK_SPACE, 0);
          load_state.frame++;
        } else if (getGameValue(D_STATE) == WALLRUN) {
          load_state.frame++;
        }

        break;
      case 1:
        if (getGameValue(D_STATE) == IN_AIR) {
          setGameValue(D_FALL_Y, -2147483647);
          setPosition(load_state.before.x, load_state.before.y,
                      load_state.before.z);
          setRotation(load_state.before.rx, load_state.before.ry);
          load_state.frame++;
        } else if (getGameValue(D_STATE) == WALLRUN) {
          load_state.frame++;
        }

        break;
      case 2:
        if (getGameValue(D_STATE) == WALLRUN) {
          loadState(&load_state.state);
          load_state.load = load_state.frame = 0;
        }

        break;
    }
  }

  if (dGetPipeValue("god")) {
    setGameValue(D_HEALTH, 100);
    setGameValue(D_FALL_Y, floatToInt(-2147483647));
  }

  if (dGetPipeValue("float")) {
    if (focused && !getGameValue(D_PAUSED)) {
      float rx = gameRotToDeg(getGameValue(D_ROTATION_X));
      float s  = 50;

      float_data.vx = float_data.vy = float_data.vz = 0;

      int forward  = dGetPipeValue("forward"),
          backward = dGetPipeValue("backward");

      if (backward) {
        rx += 180;
      }

      if (dGetPipeValue("left")) {
        rx -= backward ? -45 : forward ? 45 : 90;
      }

      if (dGetPipeValue("right")) {
        rx += backward ? -45 : forward ? 45 : 90;
      }

      if (dGetPipeValue("up")) {
        float_data.vy = s;
      }

      if (dGetPipeValue("down")) {
        float_data.vy = -s;
      }

      if (forward || gameRotToDeg(getGameValue(D_ROTATION_X)) != rx) {
        float_data.vx = cos(rx * PI / 180) * s;
        float_data.vz = sin(rx * PI / 180) * s;
      }

      float_data.x += float_data.vx;
      float_data.y += float_data.vy;
      float_data.z += float_data.vz;

      float vs = getVectorDist(float_data.vx, float_data.vz);
      vs *= vs;

      setGameValue(D_VELOCITY_X, floatToInt(cos(rx * PI / 180) * vs));
      setGameValue(D_VELOCITY_Y, floatToInt(
                                    sign(float_data.vy) * float_data.vy *
                                        float_data.vy));
      setGameValue(D_VELOCITY_Z, floatToInt(sin(rx * PI / 180) * vs));
    } else {
      setGameValue(D_VELOCITY_X, 0);
      setGameValue(D_VELOCITY_Y, 0);
      setGameValue(D_VELOCITY_Z, 0);
    }

    setGameValue(D_POSITION_X, floatToInt(float_data.x));
    setGameValue(D_POSITION_Y, floatToInt(float_data.y));
    setGameValue(D_POSITION_Z, floatToInt(float_data.z));

    setGameValue(D_STATE, GROUNDED - 1);
    setGameValue(D_FALL_Y, floatToInt(-2147483647));
  }
}

int WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
    suspendGame();
    // initEffects();
    initSpeedMod();
    installHooks();
    resumeGame();

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, &hinstDll, 0,
                 NULL);
  }

  return 1;
}
