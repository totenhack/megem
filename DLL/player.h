#pragma once

#define PLAYER_COLLISION 0x40
#define PLAYER_STATE 0x68
#define PLAYER_MOVEMENT_STATE 0x4FE
#define PLAYER_WALKING_STATE 0x503
#define PLAYER_ACTION_STATE 0x505
#define PLAYER_HAND_STATE 0x4F4
#define PLAYER_WALLRUN_STATE 0x2A0
#define PLAYER_WALL_VZ 0x114
#define PLAYER_WALL_STATE 0x2A8
#define PLAYER_X 0xE8
#define PLAYER_Y 0xEC
#define PLAYER_Z 0xF0
#define PLAYER_VX 0x100
#define PLAYER_VY 0x104
#define PLAYER_VZ 0x108
#define PLAYER_RX 0xF4
#define PLAYER_RY 0xF8
#define PLAYER_RZ 0xFC
#define PLAYER_SCALE 0x154
#define PLAYER_SX 0x158
#define PLAYER_SY 0x15C
#define PLAYER_SZ 0x160
#define PLAYER_MAX_GROUND_SPEED 0x264
#define PLAYER_HEALTH 0x2B8
#define PLAYER_LX 0x2E0
#define PLAYER_LY 0x2E4
#define PLAYER_LZ 0x2E8
#define PLAYER_OX 0x5CC
#define PLAYER_OY 0x5D0
#define PLAYER_OZ 0x5D4
#define PLAYER_PX 0x4CC
#define PLAYER_PY 0x4D0
#define PLAYER_PZ 0x4D4
#define PLAYER_FY 0x72C

#define CAMERA_RX 0xF4
#define CAMERA_RY 0xF8
#define CAMERA_RZ 0xFC
#define CAMERA_LOCK 0x2FC
#define CAMERA_REACTION_TIME 0x5A8

void SetupPlayer();
DWORD GetPlayerBase();
DWORD GetCameraBase();
float IntToDegrees(int i);
int DegreesToInt(float i);