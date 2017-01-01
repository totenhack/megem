#define DIALOG_WIDTH 250
#define DIALOG_HEIGHT 100
#define BUTTON_WIDTH 75
#define BUTTON_HEIGHT 25

char actions[][256] = {
  "settings",
  "states",
  "exit",
  "focus",
  "pexit",
  "psuspend",
  "presume",
  "ppause",
  "punpause",
  "padvance",
  "pspeed",
  "gspeed",
  "gpause",
  "gunpause",
  "gadvance",
  "x",
  "y",
  "z",
  "vx",
  "vy",
  "vz",
  "hv",
  "vv",
  "rx",
  "ry",
  "save0",
  "save1",
  "save2",
  "save3",
  "save4",
  "save5",
  "save6",
  "save7",
  "save8",
  "save9",
  "load0",
  "load1",
  "load2",
  "load3",
  "load4",
  "load5",
  "load6",
  "load7",
  "load8",
  "load9",
  "save",
  "load",
  "god",
  "float",
  "kg",
  "reactiontime",
  "forward",
  "backward",
  "left",
  "right",
  "up",
  "down"
};

enum {
  M_SETTINGS = 0,
  M_STATES,
  M_EXIT,
  M_FOCUS,
  M_PROCESS_EXIT,
  M_SUSPEND,
  M_RESUME,
  M_PROCESS_PAUSE,
  M_PROCESS_UNPAUSE,
  M_PROCESS_ADVANCE,
  M_PROCESS_SPEED,
  M_GAME_SPEED,
  M_GAME_PAUSE,
  M_GAME_UNPAUSE,
  M_GAME_ADVANCE,
  M_POSITION_X,
  M_POSITION_Y,
  M_POSITION_Z,
  M_VELOCITY_X,
  M_VELOCITY_Y,
  M_VELOCITY_Z,
  M_VELOCITY_H,
  M_VELOCITY_V,
  M_ROTATION_X,
  M_ROTATION_Y,
  M_SAVE_0,
  M_SAVE_1,
  M_SAVE_2,
  M_SAVE_3,
  M_SAVE_4,
  M_SAVE_5,
  M_SAVE_6,
  M_SAVE_7,
  M_SAVE_8,
  M_SAVE_9,
  M_LOAD_0,
  M_LOAD_1,
  M_LOAD_2,
  M_LOAD_3,
  M_LOAD_4,
  M_LOAD_5,
  M_LOAD_6,
  M_LOAD_7,
  M_LOAD_8,
  M_LOAD_9,
  M_SAVE_FILE,
  M_LOAD_FILE,
  M_GOD_MODE,
  M_FLOAT_MODE,
  M_KG,
  M_REACTION_TIME,
  M_COUNT
};

void menuMain(HWND hWnd) {
  HMENU menu = CreateMenu();

  HMENU file = CreateMenu();
  HMENU process = CreateMenu();
  HMENU game = CreateMenu();
  HMENU player = CreateMenu();
  HMENU position = CreateMenu();
  HMENU velocity = CreateMenu();
  HMENU rotation = CreateMenu();
  HMENU state = CreateMenu();
  HMENU tools = CreateMenu();

  // File
  AppendMenu(file, MF_STRING, M_SETTINGS, "Settings");
  AppendMenu(file, MF_STRING, M_STATES, "States");
  AppendMenu(file, MF_SEPARATOR, 0, 0);
  AppendMenu(file, MF_STRING, M_EXIT, "Exit");

  // Process
  AppendMenu(process, MF_STRING, M_FOCUS, "Focus");
  AppendMenu(process, MF_STRING, M_PROCESS_EXIT, "Exit");
  AppendMenu(process, MF_SEPARATOR, 0, 0);
  AppendMenu(process, MF_STRING, M_SUSPEND, "Suspend");
  AppendMenu(process, MF_STRING, M_RESUME, "Resume");
  AppendMenu(process, MF_SEPARATOR, 0, 0);
  AppendMenu(process, MF_STRING, M_PROCESS_PAUSE, "Pause");
  AppendMenu(process, MF_STRING, M_PROCESS_UNPAUSE, "Unpause");
  AppendMenu(process, MF_SEPARATOR, 0, 0);
  AppendMenu(process, MF_STRING, M_PROCESS_ADVANCE, "Advance Frame");
  AppendMenu(process, MF_STRING, M_PROCESS_SPEED, "Speed");

  // Game
  AppendMenu(game, MF_STRING, M_GAME_SPEED, "Speed");
  AppendMenu(game, MF_SEPARATOR, 0, 0);
  AppendMenu(game, MF_STRING, M_GAME_PAUSE, "Pause");
  AppendMenu(game, MF_STRING, M_GAME_UNPAUSE, "Unpause");
  AppendMenu(game, MF_SEPARATOR, 0, 0);
  AppendMenu(game, MF_STRING, M_GAME_ADVANCE, "Advance Frame");

  // Player
  AppendMenu(position, MF_STRING, M_POSITION_X, "X");
  AppendMenu(position, MF_STRING, M_POSITION_Y, "Y");
  AppendMenu(position, MF_STRING, M_POSITION_Z, "Z");
  AppendMenu(player, MF_POPUP, (UINT_PTR)position, "Position");
  AppendMenu(velocity, MF_STRING, M_VELOCITY_X, "X");
  AppendMenu(velocity, MF_STRING, M_VELOCITY_Y, "Y");
  AppendMenu(velocity, MF_STRING, M_VELOCITY_Z, "Z");
  AppendMenu(velocity, MF_STRING, M_VELOCITY_H, "H");
  AppendMenu(velocity, MF_STRING, M_VELOCITY_V, "V");
  AppendMenu(player, MF_POPUP, (UINT_PTR)velocity, "Velocity");
  AppendMenu(rotation, MF_STRING, M_ROTATION_X, "X");
  AppendMenu(rotation, MF_STRING, M_ROTATION_Y, "Y");
  AppendMenu(player, MF_POPUP, (UINT_PTR)rotation, "Rotation");

  // State
  HMENU save = CreateMenu();
  HMENU load = CreateMenu();
  for (int i = 0; i < 10; i++) {
    char t[7];
    sprintf(t, "Slot %d", i);
    AppendMenu(save, MF_STRING, M_SAVE_0 + i, t);
    AppendMenu(load, MF_STRING, M_LOAD_0 + i, t);
  }

  AppendMenu(state, MF_POPUP, (UINT_PTR)save, "Save");
  AppendMenu(state, MF_POPUP, (UINT_PTR)load, "Load");
  AppendMenu(state, MF_SEPARATOR, 0, 0);
  AppendMenu(state, MF_STRING, M_SAVE_FILE, "Save File");
  AppendMenu(state, MF_STRING, M_LOAD_FILE, "Load File");

  // Tools
  AppendMenu(tools, MF_POPUP, (UINT_PTR)M_GOD_MODE, "God Mode");
  AppendMenu(tools, MF_POPUP, (UINT_PTR)M_FLOAT_MODE, "Float Mode");
  AppendMenu(tools, MF_SEPARATOR, 0, 0);
  AppendMenu(tools, MF_POPUP, (UINT_PTR)M_KG, "Kick Glitch");
  AppendMenu(tools, MF_POPUP, (UINT_PTR)M_REACTION_TIME, "Reaction Time");

  // Append sub-menus to main menu

  AppendMenu(menu, MF_POPUP, (UINT_PTR)file, "File");
  AppendMenu(menu, MF_POPUP, (UINT_PTR)process, "Process");
  AppendMenu(menu, MF_POPUP, (UINT_PTR)game, "Game");
  AppendMenu(menu, MF_POPUP, (UINT_PTR)player, "Player");
  AppendMenu(menu, MF_POPUP, (UINT_PTR)state, "State");
  AppendMenu(menu, MF_POPUP, (UINT_PTR)tools, "Tools");

  SetMenu(hWnd, menu);
}
