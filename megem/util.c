#define PI 3.14159265358979323846
#define SPACEBAR 0x20
#define SHIFT 0xA0

int isnumber(char *str, unsigned int len) {
  int i, d, m;

  for (i = d = 0; i < len; i++) {
    if (str[i] < 33 || str[i] > 126 || isblank(str[i])) {
      continue;
    }

    if (str[i] == '.') {
      if (++d > 1) {
        return 0;
      }

      continue;
    }

    if (str[i] == '-') {
      if (++m > 1) {
        return 0;
      }

      continue;
    }

    if (!isdigit(str[i])) {
      return 0;
    }
  }

  return 1;
}

float intToFloat(int i) {
  float f;
  memcpy(&f, &i, 4);
  return f;
}

int floatToInt(float f) {
  int i;
  memcpy(&i, &f, 4);
  return i;
}

void checkMenu(HWND hWnd, unsigned int item) {
  MENUITEMINFO mii = { .cbSize = sizeof(MENUITEMINFO) };
  mii.fMask = MIIM_STATE;
  GetMenuItemInfo(GetMenu(hWnd), item, 0, &mii);
  mii.fState ^= MFS_CHECKED;
  SetMenuItemInfo(GetMenu(hWnd), item, 0, &mii);
}

void uncheckMenu(HWND hWnd, unsigned int item) {
  MENUITEMINFO mii = { .cbSize = sizeof(MENUITEMINFO) };
  mii.fMask = MIIM_STATE;
  GetMenuItemInfo(GetMenu(hWnd), item, 0, &mii);
  mii.fState ^= MFS_UNCHECKED;
  SetMenuItemInfo(GetMenu(hWnd), item, 0, &mii);
}

int isMenuChecked(HWND hWnd, unsigned int item) {
  MENUITEMINFO mii = { .cbSize = sizeof(MENUITEMINFO) };
  mii.fMask = MIIM_STATE;
  GetMenuItemInfo(GetMenu(hWnd), item, 0, &mii);

  return mii.fState == MFS_CHECKED;
}

void drawText(HDC hdc, int x, int y, const char *format, ...) {
  static char buffer[MAX_PATH];
  int length = 0;

  va_list arg;
  va_start(arg, format);
  length += vsprintf(buffer, format, arg);
  va_end(arg);

  ExtTextOut(hdc, x, y, ETO_CLIPPED, 0, buffer, length, NULL);
}

void openFileInWindow(char *path) {
  char arg[MAX_PATH];
  strcpy(arg, "/n,\"");
  strcat(arg, path);
  strcat(arg, "\"");
  ShellExecute(0, "open", "explorer.exe", arg, 0, SW_NORMAL);
}
