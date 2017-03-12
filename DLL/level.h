#pragma once

void SetupLevel();
void LevelStream(int id, char load);
void GetStringById(int id, char *out);
int GetIdByString(char *str);
void GetSublevels();
char GetSublevelStatus(char *name);