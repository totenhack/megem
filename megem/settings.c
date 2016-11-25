LRESULT CALLBACK settingsProc(HWND, UINT, WPARAM, LPARAM);

void settings(HWND parent) {
  HWND hWnd = CreateWindow("#32770", "Settings",
                           WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                           CW_USEDEFAULT, CW_USEDEFAULT, 600, 450,
                           parent, 0, GetModuleHandle(0), NULL);

  ShowWindow(hWnd, 1);
  UpdateWindow(hWnd);

  SetWindowLong(hWnd, GWL_WNDPROC, (LONG)settingsProc);
}

LRESULT CALLBACK settingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

  return DefWindowProc(hWnd, msg, wParam, lParam);
}
