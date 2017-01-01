#define EXTENSION "megem_\0"

void setPipeValue(char *name, const char *format, ...) {
  char buffer[512];

  va_list arg;
  va_start(arg, format);
  vsprintf(buffer, format, arg);
  va_end(arg);

  char path[MAX_PATH];
  unsigned int length = GetTempPath(MAX_PATH, path);
  strcpy(path + length, EXTENSION);
  strcpy(path + strlen(path), name);

  FILE *file = fopen(path, "w");
  fprintf(file, "%s", buffer);
  fclose(file);
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

int dGetPipeValue(char *name) {
  char out[BUFSIZE];
  getPipeValue(name, out);

  return atoi(out);
}


float fGetPipeValue(char *name) {
  char out[BUFSIZE];
  getPipeValue(name, out);

  return atof(out);
}
