#pragma once

#define ENGINE_SPEED 0xC2C
#define ENGINE_PAUSED 0xC54

void SetupEngine();
void KG(bool);
void Strang(bool);
void Beamer(bool);
void God(bool);
void Float(bool);
DWORD GetEngineBase();
void OpenDebugConsole();
void CloseDebugConsole();
bool Loading();