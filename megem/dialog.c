#define OKAY 1
#define CANCEL 2
#define INPUT 3

#define PROCARGS HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam

int isnumber();

int dialog(HWND parent, char *prompt, void *proc) {
  HWND hWnd = CreateWindow("#32770", prompt,
                           WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                           CW_USEDEFAULT, CW_USEDEFAULT, DIALOG_WIDTH,
                           DIALOG_HEIGHT, parent, NULL, GetModuleHandle(0),
                           NULL);

  HWND okay = CreateWindow("BUTTON", "Ok", WS_VISIBLE | WS_CHILD | WS_BORDER,
                           DIALOG_WIDTH - (BUTTON_WIDTH * 1.25),
                           (DIALOG_HEIGHT / 2) - (BUTTON_HEIGHT * 1.65),
                           BUTTON_WIDTH,
                           BUTTON_HEIGHT, hWnd, (HMENU)OKAY, GetModuleHandle(0),
                           NULL);

  HWND cancel = CreateWindow("BUTTON", "Cancel", WS_VISIBLE | WS_CHILD |
                             WS_BORDER,
                             DIALOG_WIDTH - (BUTTON_WIDTH * 1.25),
                             (DIALOG_HEIGHT / 2) - (BUTTON_HEIGHT * 0.35),
                             BUTTON_WIDTH,
                             BUTTON_HEIGHT, hWnd, (HMENU)CANCEL,
                             GetModuleHandle(0), NULL);

  HWND input = CreateWindow("EDIT", "LMAO", WS_VISIBLE | WS_CHILD | WS_BORDER,
                            BUTTON_WIDTH * 0.25,
                            DIALOG_HEIGHT / 2 - (BUTTON_HEIGHT),
                            BUTTON_WIDTH * 1.5,
                            BUTTON_HEIGHT, hWnd, (HMENU)INPUT,
                            GetModuleHandle(0), NULL);

  ShowWindow(hWnd, 1);
  UpdateWindow(hWnd);

  ShowWindow(okay, 1);
  UpdateWindow(okay);

  ShowWindow(cancel, 1);
  UpdateWindow(cancel);

  ShowWindow(input, 1);
  UpdateWindow(input);

  SetWindowLong(hWnd, GWL_WNDPROC, (LONG)proc);
  LRESULT(CALLBACK *procedure)(HWND, UINT, WPARAM, LPARAM) = proc;
  procedure(hWnd, WM_CREATE, 0, 0);
}

void getInputVal(HWND hWnd, char *out) {
  unsigned int len = GetWindowText(GetDlgItem(hWnd, INPUT), out, BUFSIZE);
  out[len] = '\0';

  if (!isnumber(out, len)) {
    MessageBox(0, "Input must be a number", "Error", MB_ICONWARNING);
  }
}

LRESULT CALLBACK setProcessSpeedProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setProcessSpeed(atof(buffer));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    getPipeValue("speed", buffer);
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setGameSpeedProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_GAME_SPEED, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_GAME_SPEED)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setPositionXProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_POSITION_X, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_POSITION_X)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setPositionYProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_POSITION_Y, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_POSITION_Y)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setPositionZProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_POSITION_Z, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_POSITION_Z)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setVelocityXProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_VELOCITY_X, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_VELOCITY_X)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setVelocityYProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_VELOCITY_Y, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_VELOCITY_Y)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setVelocityZProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_VELOCITY_Z, floatToInt(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", intToFloat(getGameValue(D_VELOCITY_Z)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setVelocityHProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        float speed = kmhToV(atof(buffer));
        float vx = intToFloat(getGameValue(D_VELOCITY_X)),
              vz = intToFloat(getGameValue(D_VELOCITY_Z));

        float rx;

        if (vx + vz == 0) {
          rx = gameRotToDeg(getGameValue(D_ROTATION_X)) * PI / 180;
        } else {
          rx = atan2(vz, vx);
          rx = (rx > 0 ? rx : ((float)2 * PI + rx)) * (float)360 / (2 * PI);
          rx *= PI / 180;
        }

        setGameValue(D_VELOCITY_X, floatToInt(speed * cos(rx)));
        setGameValue(D_VELOCITY_Z, floatToInt(speed * sin(rx)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    float x = intToFloat(getGameValue(D_VELOCITY_X)),
          z = intToFloat(getGameValue(D_VELOCITY_Z));

    sprintf(buffer, "%f", vToKmh(getVectorDist(x, z)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setVelocityVProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_VELOCITY_Y, floatToInt(kmhToV(atof(buffer))));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    float y = vToKmh(intToFloat(getGameValue(D_VELOCITY_Y)));

    sprintf(buffer, "%f", y);
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setRotationXProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_ROTATION_X, degToGameRot(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", gameRotToDeg(getGameValue(D_ROTATION_X)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK setRotationYProc(PROCARGS) {
  char buffer[BUFSIZE];

  if (msg == WM_COMMAND) {
    switch(LOWORD(wParam)) {
      case OKAY:
        getInputVal(hWnd, buffer);
        setGameValue(D_ROTATION_Y, degToGameRot(atof(buffer)));
      case CANCEL:
        DestroyWindow(hWnd);
        break;
    }
  } else if (msg == WM_CREATE) {
    sprintf(buffer, "%f", gameRotToDeg(getGameValue(D_ROTATION_Y)));
    SetWindowText(GetDlgItem(hWnd, INPUT), buffer);
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}
