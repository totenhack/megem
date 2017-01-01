struct {
  HWND hWnd;
  int width, height;
  DWORD *buffer;

  BITMAPINFO bmi;
} effects;

void initEffects() {
  effects.hWnd = getGameWindow();

  RECT rc;
  GetClientRect(effects.hWnd, &rc);
  effects.width = rc.right - rc.left;
  effects.height = rc.bottom - rc.top;
  effects.buffer = malloc(effects.width * effects.height * sizeof(DWORD));

  effects.bmi.bmiHeader.biSize          = sizeof(BITMAPINFO);
  effects.bmi.bmiHeader.biPlanes        = 1;
  effects.bmi.bmiHeader.biBitCount      = 32;
  effects.bmi.bmiHeader.biCompression   = BI_RGB;
  effects.bmi.bmiHeader.biSizeImage     = 0;
  effects.bmi.bmiHeader.biClrUsed       = 0;
  effects.bmi.bmiHeader.biClrImportant  = 0;
  effects.bmi.bmiHeader.biXPelsPerMeter = 2480;
  effects.bmi.bmiHeader.biYPelsPerMeter = 2480;
  effects.bmi.bmiHeader.biWidth         = effects.width;
  effects.bmi.bmiHeader.biHeight        = effects.height;
}

void blur(DWORD *scl, DWORD *tcl, float w, float h, float r) {
    float rs = ceil(r * 2.57);     // significant radius
    for(float i=0; i<h; i++)
        for(float j=0; j<w; j++) {
            float val = 0, wsum = 0;
            for(float iy = i-rs; iy<i+rs+1; iy++)
                for(float ix = j-rs; ix<j+rs+1; ix++) {
                    float x = min(w-1, max(0, ix));
                    float y = min(h-1, max(0, iy));
                    float dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                    float wght = exp( -dsq / (2*r*r) ) / (PI*2*r*r);
                    val += scl[(int)(y*w+x)] * wght;  wsum += wght;
                }
            tcl[(int)(i*w+j)] = round(val/wsum);
        }
}

void effectsLoop() {
  HDC hdc = GetDC(NULL);
  HDC hdcCopy = CreateCompatibleDC(hdc);
  HBITMAP bitmap = CreateCompatibleBitmap(hdc, effects.width, effects.height);

  SelectObject(hdcCopy, bitmap);
  PrintWindow(effects.hWnd, hdcCopy, PW_CLIENTONLY);

  GetBitmapBits(bitmap, effects.width * effects.height * sizeof(DWORD), effects.buffer);

  DeleteDC(hdcCopy);
  DeleteObject(bitmap);

  int size = effects.width * effects.height;
  for (int i = 0; i < size / 2; i++) {
    int y = i / effects.width;
    int x = effects.width - (i % effects.width);
    int opposite = size - x - (y * effects.width);

    DWORD temp = effects.buffer[i];
    effects.buffer[i] = effects.buffer[opposite];
    effects.buffer[opposite] = temp;

    /* if (GetBValue(effects.buffer[i]) > 225) {
      effects.buffer[i] = RGB(255, 255, 255);
    }

    if (GetBValue(effects.buffer[opposite]) > 225) {
      effects.buffer[opposite] = RGB(255, 255, 255);
    } */
  }

  DWORD *dest = malloc(effects.width * effects.height * sizeof(DWORD));
  blur(effects.buffer, dest, effects.width, effects.height, 0);

  SetDIBitsToDevice(hdc, 0, 0, effects.width, effects.height, 0, 0, 0, effects.height, dest, &effects.bmi, DIB_RGB_COLORS);

  free(dest);

  ReleaseDC(NULL, hdc);
}

/* void effectsLoop() {
  HWND window = getGameWindow();
  HDC hdc = GetDC(window);
  RECT rc;
  GetClientRect(window, &rc);

  int width = rc.right - rc.left;
  int height = rc.bottom - rc.top;

  static BITMAPINFO bmi = {
    .bmiHeader.biSize          = sizeof(BITMAPINFO),
    .bmiHeader.biPlanes        = 1,
    .bmiHeader.biBitCount      = 32,
    .bmiHeader.biCompression   = BI_RGB,
    .bmiHeader.biSizeImage     = 0,
    .bmiHeader.biClrUsed       = 0,
    .bmiHeader.biClrImportant  = 0,
    .bmiHeader.biXPelsPerMeter = 2480,
    .bmiHeader.biYPelsPerMeter = 2480
  };

  bmi.bmiHeader.biWidth  = width;
  bmi.bmiHeader.biHeight = height;

  static DWORD *buffer;
  if (!buffer) {
    buffer = malloc(width * height * sizeof(DWORD));
  }

  HDC hdcScreen = GetDC(NULL);
  HDC hdcCopy = CreateCompatibleDC(hdcScreen);
  HBITMAP bitmap = CreateCompatibleBitmap(hdcScreen, width, height);

  SelectObject(hdcCopy, bitmap);
  PrintWindow(window, hdcCopy, PW_CLIENTONLY);

  GetBitmapBits(bitmap, width * height * sizeof(DWORD), buffer);

  DeleteDC(hdc);
  DeleteObject(bitmap);
  ReleaseDC(NULL, hdcScreen);

  int size = width * height;
  for (int i = 0; i < size / 2; i++) {
    int y = i / width;
    int x = width - (i % width);
    int opposite = size - x - (y * width);

    DWORD temp = buffer[i];
    buffer[i] = buffer[opposite];
    buffer[opposite] = temp;

    if (GetRValue(buffer[i]) > 150) {
      buffer[i] = 0;
    }

    if (GetRValue(buffer[opposite]) > 150) {
      buffer[opposite] = 0;
    }
  }

  SetDIBitsToDevice(hdc, 0, 0, width, height, 0, 0, 0, height, buffer, &bmi, DIB_RGB_COLORS);

  // free(buffer);
} */
