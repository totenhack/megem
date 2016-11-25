#define EXTENSION "megem_"

int initPipe() {
  CloseHandle(process.pipe);
  LPTSTR pipeName = "\\\\.\\pipe\\megem";

  for (;;) {
    process.pipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                              OPEN_EXISTING, 0, NULL);

    if (process.pipe != INVALID_HANDLE_VALUE) {
      break;
    }

    if (!WaitNamedPipe(pipeName, 20000)) {
      return 0;
    }
  }

  return 1;
}

void sendPipeMessage(const char *format, ...) {
  char buffer[BUFSIZE];

  va_list arg;
  va_start(arg, format);
  vsprintf(buffer, format, arg);
  va_end(arg);

  WriteFile(process.pipe, buffer, strlen(buffer), NULL, NULL);
  FlushFileBuffers(process.pipe);
}

void getPipeValue(char *name, char *out) {
  char path[MAX_PATH];
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, EXTENSION);
  strcpy(path + strlen(path), name);

  FILE *file = fopen(path, "r");
  fscanf(file, "%s", out);
  fclose(file);
}

float fGetPipeValue(char *name) {
  char out[BUFSIZE];
  getPipeValue(name, out);

  return atof(out);
}