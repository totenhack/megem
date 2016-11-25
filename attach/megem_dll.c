#include <stdio.h>
#include <windows.h>

#define BUFSIZE 512

int kg_toggle     = 0,
    beamer_toggle = 0,
    strang_toggle = 0;

#include "game.c"
#include "pipe.c"
#include "hook.c"
#include "speed_mod.c"

int WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
void newClient(LPVOID);
void doCommand(char *);
void loop();

void MainThread(HINSTANCE dll) {
  FreeLibrary((HMODULE)dll);

  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)loop, NULL, 0, 0);

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
  char *bufferIn = (char *)malloc(BUFSIZE);
  DWORD bufferInLength;

  int success;
  for (;;) {
    success = ReadFile(pipe, bufferIn, BUFSIZE, &bufferInLength, NULL);

    if (!success) {
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
    // Process speed
    case 's':
      setSpeed(atof(command));
      break;
    // Advance frame
    case 'a':
      advanceFrame();
      break;
    // Kg
    case 'k':
      if (!kg_toggle && !getGameValue(D_PAUSED) &&
          (getGameValue(D_STATE) == IN_AIR || getGameValue(D_STATE) == WALLRUN))
      {
        kg_toggle = 1;
      }
      break;/*
    // Beamer
    case 'b':
      beamer_toggle = !beamer_toggle;
      break;
    // Strang
    case 't':
      strang_toggle = !strang_toggle;
      break; */
  }
}

void loop() {
  for (;;) {
    if (kg_toggle) {
      DWORD state = getGameValue(D_STATE);

      if (!(state == WALLRUN || state == GROUNDED || state == IN_AIR)) {
        kg_toggle = 0;
      } else if (state == WALLRUN) {
        SendMessage(getGameWindow(), WM_LBUTTONDOWN, MK_LBUTTON, 0);
        SendMessage(getGameWindow(), WM_LBUTTONUP, MK_LBUTTON, 0);
      } else if (state == GROUNDED) {
        SendMessage(getGameWindow(), WM_KEYDOWN, VK_SPACE, VK_SPACE);
        SendMessage(getGameWindow(), WM_KEYUP, VK_SPACE, VK_SPACE);
        kg_toggle = 0;
      }
    }

    Sleep(1);
  }
}

int WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
    initSpeedMod();

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, &hinstDll, 0,
                 NULL);
  }

  return 1;
}
