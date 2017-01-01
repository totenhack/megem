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

typedef struct {
  char name[255];
  short code;
} KEY;

static KEY KEYS[] = {
  { "lmb", 0x01 },
  { "leftmousebutton", 0x01 },
  { "left_mouse_button", 0x01 },
  { "rmb", 0x02 },
  { "rightmousebutton", 0x02 },
  { "right_mouse_button", 0x02 },
  { "cancel", 0x03 },
  { "mmb", 0x04 },
  { "middlemousebutton", 0x04 },
  { "middle_mouse_button", 0x04 },
  { "x1mb", 0x05 },
  { "x1mousebutton", 0x05 },
  { "x1_mouse_button", 0x05 },
  { "x2mb", 0x06 },
  { "x2mousebutton", 0x06 },
  { "x2_mouse_button", 0x06 },
  { "undefined", 0x07 },
  { "backspace", 0x08 },
  { "back", 0x08 },
  { "tab", 0x09 },
  { "clear", 0x0C },
  { "return", 0x0D },
  { "enter", 0x0D },
  { "shift", 0x10 },
  { "control", 0x11 },
  { "ctrl", 0x11 },
  { "menu", 0x12 },
  { "alt", 0x12 },
  { "pause", 0x13 },
  { "capital", 0x14 },
  { "capslock", 0x14 },
  { "caps", 0x14 },
  { "caps_lock", 0x14 },
  { "kana", 0x15 },
  { "hanguel", 0x15 },
  { "junja", 0x17 },
  { "final", 0x18 },
  { "hanja", 0x19 },
  { "kanji", 0x19 },
  { "escape", 0x1B },
  { "esc", 0x1B },
  { "convert", 0x1C },
  { "nonconvert", 0x1D },
  { "accept", 0x1E },
  { "modechange", 0x1F },
  { "mode_change", 0x1F },
  { "space", 0x20 },
  { "spacebar", 0x20 },
  { "pageup", 0x21 },
  { "page_up", 0x21 },
  { "pagedown", 0x22 },
  { "page_down", 0x22 },
  { "end", 0x23 },
  { "home", 0x24 },
  { "left", 0x25 },
  { "left_arrow", 0x25 },
  { "leftarrow", 0x25 },
  { "up", 0x26 },
  { "up_arrow", 0x26 },
  { "uparrow", 0x26 },
  { "right", 0x27 },
  { "right_arrow", 0x27 },
  { "rightarrow", 0x27 },
  { "down", 0x28 },
  { "down_arrow", 0x28 },
  { "downarrow", 0x28 },
  { "select", 0x29 },
  { "print", 0x2A },
  { "execute", 0x2B },
  { "printscreen", 0x2C },
  { "prtscr", 0x2C },
  { "print_screen", 0x2C },
  { "insert", 0x2D },
  { "delete", 0x2E },
  { "help", 0x2F },
  { "0", 0x30 },
  { "1", 0x31 },
  { "2", 0x32 },
  { "3", 0x33 },
  { "4", 0x34 },
  { "5", 0x35 },
  { "6", 0x36 },
  { "7", 0x37 },
  { "8", 0x38 },
  { "9", 0x39 },
  { "a", 0x41 },
  { "b", 0x42 },
  { "c", 0x43 },
  { "d", 0x44 },
  { "e", 0x45 },
  { "f", 0x46 },
  { "g", 0x47 },
  { "h", 0x48 },
  { "i", 0x49 },
  { "j", 0x4A },
  { "k", 0x4B },
  { "l", 0x4C },
  { "m", 0x4D },
  { "n", 0x4E },
  { "o", 0x4F },
  { "p", 0x50 },
  { "q", 0x51 },
  { "r", 0x52 },
  { "s", 0x53 },
  { "t", 0x54 },
  { "u", 0x55 },
  { "v", 0x56 },
  { "w", 0x57 },
  { "x", 0x58 },
  { "y", 0x59 },
  { "z", 0x5A },
  { "lwin", 0x5B },
  { "leftwindowskey", 0x5B },
  { "left_windows_key", 0x5B },
  { "rwin", 0x5C },
  { "rightwindowskey", 0x5C },
  { "right_windows_key", 0x5C },
  { "apps", 0x5D },
  { "sleep", 0x5F },
  { "numpad0", 0x60 },
  { "numpad1", 0x61 },
  { "numpad2", 0x62 },
  { "numpad3", 0x63 },
  { "numpad4", 0x64 },
  { "numpad5", 0x65 },
  { "numpad6", 0x66 },
  { "numpad7", 0x67 },
  { "numpad8", 0x68 },
  { "numpad9", 0x69 },
  { "multiply", 0x6A },
  { "add", 0x6B },
  { "separator", 0x6C },
  { "subtract", 0x6D },
  { "decimal", 0x6E },
  { "divide", 0x6F },
  { "f1", 0x70 },
  { "f2", 0x71 },
  { "f3", 0x72 },
  { "f4", 0x73 },
  { "f5", 0x74 },
  { "f6", 0x75 },
  { "f7", 0x76 },
  { "f8", 0x77 },
  { "f9", 0x78 },
  { "f10", 0x79 },
  { "f11", 0x7A },
  { "f12", 0x7B },
  { "f13", 0x7C },
  { "f14", 0x7D },
  { "f15", 0x7E },
  { "f16", 0x7F },
  { "f17", 0x80 },
  { "f18", 0x81 },
  { "f19", 0x82 },
  { "f20", 0x83 },
  { "f21", 0x84 },
  { "f22", 0x85 },
  { "f23", 0x86 },
  { "f24", 0x87 },
  { "numberlock", 0x90 },
  { "number_lock", 0x90 },
  { "numlock", 0x90 },
  { "num_lock", 0x90 },
  { "scroll", 0x91 },
  { "lshift", 0xA0 },
  { "leftshift", 0xA0 },
  { "left_shift", 0xA0 },
  { "rshift", 0xA1 },
  { "rightshift", 0xA1 },
  { "right_shift", 0xA1 },
  { "lcontrol", 0xA2 },
  { "leftcontrol", 0xA2 },
  { "left_control", 0xA2 },
  { "rcontrol", 0xA3 },
  { "rightcontrol", 0xA3 },
  { "right_control", 0xA3 },
  { "lmenu", 0xA4 },
  { "leftmenu", 0xA4 },
  { "left_menu", 0xA4 },
  { "rmenu", 0xA5 },
  { "rightmenu", 0xA5 },
  { "right_menu", 0xA5 },
  { "+", 0xBB },
  { "plus", 0xBB },
  { ",", 0xBC },
  { "comma", 0xBC },
  { "-", 0xBD },
  { "minus", 0xBD },
  { ".", 0xBE },
  { "period", 0xBE }
};
