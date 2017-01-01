#define PI 3.14159265358979323846
#define SPACEBAR 0x20
#define SHIFT 0xA0

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

int isKeyDown(short k) {
  return GetAsyncKeyState(k) < 0;
}

int sign(float v) {
  return v < 0 ? -1 : v == 0 ? 0 : 1;
}
