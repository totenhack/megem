#include "menu.c"
#include "dialog.c"
#include "settings.c"

#define KEYDOWN 0
#define KEYUP 1

LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);
void update();
LRESULT CALLBACK keyboardHook(int, WPARAM, LPARAM);

int windowMain() {
  WNDCLASSEX wndClass = {
    .cbSize        = sizeof(WNDCLASSEX),
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = windowProc,
    .cbClsExtra    = 0,
    .cbWndExtra    = 0,
    .hInstance     = GetModuleHandle(NULL),
    .hIcon         = LoadIcon(wndClass.hInstance,
                              MAKEINTRESOURCE(IDI_APPLICATION)),
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName  = NULL,
    .lpszClassName = "win32app",
    .hIconSm       = LoadIcon(wndClass.hInstance,
                              MAKEINTRESOURCE(IDI_APPLICATION))
  };

  if (!RegisterClassEx(&wndClass)) {
    return 0;
  }

  process.hWnd = CreateWindow(wndClass.lpszClassName, "Megem",
                           WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                           CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, 0, 0,
                           GetModuleHandle(0), 0);

  ShowWindow(process.hWnd, 1);
  UpdateWindow(process.hWnd);

  process.hdc = GetDC(process.hWnd);

  SetTimer(NULL, 1, 34, (TIMERPROC)update);

  process.keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, &keyboardHook, 0, 0);
  MSG msg;
  while (msg.message != WM_QUIT) {
    if (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

void cleanExit() {
  UnhookWindowsHookEx(process.keyboard_hook);
  setProcessSpeed(1);
  exit(0);
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  unsigned int menu = LOWORD(wParam);

  switch (msg) {
    case WM_CREATE:
      menuMain(hWnd);
      break;
    case WM_CLOSE:
      cleanExit();
      break;
    case WM_COMMAND:
      switch (menu) {
        case M_SETTINGS:
          settings(hWnd);
          break;
        case M_STATES: {
          char path[MAX_PATH];
          unsigned int length = GetTempPath(MAX_PATH, path);
          path[length] = '\0';

          openFileInWindow(path);

          break;
        }
        case M_EXIT:
          cleanExit();
          break;
        case M_FOCUS:
          focusGame();
          break;
        case M_SUSPEND:
          suspendGame();
          break;
        case M_RESUME:
          resumeGame();
          break;
        case M_PROCESS_PAUSE:
          pauseProcess(1);
          break;
        case M_PROCESS_UNPAUSE:
          pauseProcess(0);
          break;
        case M_PROCESS_ADVANCE:
          sendPipeMessage("a");
          break;
        case M_PROCESS_SPEED:
          dialog(hWnd, "Process Speed", setProcessSpeedProc);
          break;
        case M_GAME_SPEED:
          dialog(hWnd, "Game Speed", setGameSpeedProc);
          break;
        case M_POSITION_X:
          dialog(hWnd, "Position X", setPositionXProc);
          break;
        case M_POSITION_Y:
          dialog(hWnd, "Position Y", setPositionYProc);
          break;
        case M_POSITION_Z:
          dialog(hWnd, "Position Z", setPositionZProc);
          break;
        case M_VELOCITY_X:
          dialog(hWnd, "Velocity X", setVelocityXProc);
          break;
        case M_VELOCITY_Y:
          dialog(hWnd, "Velocity Y", setVelocityYProc);
          break;
        case M_VELOCITY_Z:
          dialog(hWnd, "Velocity Z", setVelocityZProc);
          break;
        case M_ROTATION_X:
          dialog(hWnd, "Rotation X", setRotationXProc);
          break;
        case M_ROTATION_Y:
          dialog(hWnd, "Rotation Y", setRotationYProc);
          break;
        case M_GAME_PAUSE:
          setGameValue(D_PAUSED, 100000000);
          break;
        case M_GAME_UNPAUSE:
          setGameValue(D_PAUSED, 0);
          break;
        case M_GAME_ADVANCE:
          setGameValue(D_PAUSED, 0);
          Sleep(17);
          setGameValue(D_PAUSED, 100000000);
          break;
        case M_SAVE_0:
        case M_SAVE_1:
        case M_SAVE_2:
        case M_SAVE_3:
        case M_SAVE_4:
        case M_SAVE_5:
        case M_SAVE_6:
        case M_SAVE_7:
        case M_SAVE_8:
        case M_SAVE_9:
          saveStateSlot(menu - M_SAVE_0);
          break;
        case M_LOAD_0:
        case M_LOAD_1:
        case M_LOAD_2:
        case M_LOAD_3:
        case M_LOAD_4:
        case M_LOAD_5:
        case M_LOAD_6:
        case M_LOAD_7:
        case M_LOAD_8:
        case M_LOAD_9:
          loadStateSlot(menu - M_LOAD_0);
          break;
        case M_SAVE_FILE: {
          char path[MAX_PATH] = "";

          OPENFILENAME name = {
            .lStructSize = sizeof(OPENFILENAME),
            .hwndOwner   = hWnd,
            .lpstrFilter = "*.medat\0",
            .lpstrFile= path,
            .nMaxFile = MAX_PATH,
            .Flags = OFN_OVERWRITEPROMPT,
            .lpstrDefExt = "medat\0"
          };

          if (GetSaveFileName(&name)) {
            fclose(fopen(path, "a"));
            saveStateFile(path);
          }

          break;
        }
        case M_LOAD_FILE: {
          char path[MAX_PATH] = "";

          OPENFILENAME name = {
            .lStructSize = sizeof(OPENFILENAME),
            .hwndOwner   = hWnd,
            .lpstrFilter = "*.medat\0",
            .lpstrFile= path,
            .nMaxFile = MAX_PATH,
            .Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
            .lpstrDefExt = "medat\0"
          };

          if (GetOpenFileName(&name)) {
            loadStateFile(path);
          }

          break;
        }
        case M_GOD_MODE:
          checkMenu(hWnd, menu);

          if (isMenuChecked(hWnd, menu)) {
            process.godmode_toggle = 1;
          } else {
            process.godmode_toggle = 0;
          }
          break;
        case M_FLOAT_MODE:
          checkMenu(hWnd, menu);

          if (isMenuChecked(hWnd, menu)) {
            setVelocity(0, 0, 0);
            saveState(&process.float_state);
            CreateThread(0, 0,(LPTHREAD_START_ROUTINE)floatMode, 0, 0, 0);
            process.floatmode_toggle = 1;
          } else {
            process.floatmode_toggle = 0;
          }
          break;
        case M_KG:
          sendPipeMessage("k");
          break;
      }

      break;
    default:
      return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return 0;
}
