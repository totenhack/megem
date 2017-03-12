#pragma once

void SetupPlayer();
int GetPlayerBase();
int GetCameraBase();
float IntToDegrees(int int_);
int DegreesToInt(float degrees);
int __fastcall UpdatePlayerHook(int, void *, int, int);