#include "stdafx.h"

char keybinds_default[] = "focus:none suspend:none pspeed0:none pspeed.25:none pspeed.5:none pspeed.75:none pspeed1:none pspeed2:none pause:none espeed0:none espeed.25:none espeed.5:none espeed.75:none espeed1:none espeed2:none god:1 fly:2 flyincrease:e flydecrease:q kg:none rt:none strang:none ss0:4 ss1:none ss2:none ss3:none ss4:none ss5:none ss6:none ss7:none ss8:none ss9:none ls0:5 ls1:none ls2:none ls3:none ls4:none ls5:none ls6:none ls7:none ls8:none ls9:none";

int getIndexByKeyCode(int keycode) {
	for (int i = 0; i < LENGTH(KEYS); i++) {
		if (KEYS[i].keycode == keycode) {
			return i;
		}
	}

	return 0;
}

INT_PTR CALLBACK KeybindsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	static bool setBind = false;
	static int id = 0;

	switch (message) {
		case WM_INITDIALOG: {
			if (!FileExists("keybinds.dat")) {
				FILE *file = fopen("keybinds.dat", "w");
				fprintf(file, "%s", keybinds_default);
				fclose(file);
			}

			char focus[0xFF], suspend[0xFF], pspeed0[0xFF], pspeed25[0xFF], pspeed5[0xFF], pspeed75[0xFF], pspeed1[0xFF], pspeed2[0xFF], pause[0xFF], espeed0[0xFF], espeed25[0xFF], espeed5[0xFF], espeed75[0xFF], espeed1[0xFF], espeed2[0xFF], god[0xFF], fly[0xFF], flyincrease[0xFF], flydecrease[0xFF], kg[0xFF], rt[0xFF], strang[0xFF], ss0[0xFF], ss1[0xFF], ss2[0xFF], ss3[0xFF], ss4[0xFF], ss5[0xFF], ss6[0xFF], ss7[0xFF], ss8[0xFF], ss9[0xFF], ls0[0xFF], ls1[0xFF], ls2[0xFF], ls3[0xFF], ls4[0xFF], ls5[0xFF], ls6[0xFF], ls7[0xFF], ls8[0xFF], ls9[0xFF];

			FILE *file = fopen("keybinds.dat", "r");
			
			fscanf(file, "focus:%s suspend:%s pspeed0:%s pspeed.25:%s pspeed.5:%s pspeed.75:%s pspeed1:%s pspeed2:%s pause:%s espeed0:%s espeed.25:%s espeed.5:%s espeed.75:%s espeed1:%s espeed2:%s god:%s fly:%s flyincrease:%s flydecrease:%s kg:%s rt:%s strang:%s ss0:%s ss1:%s ss2:%s ss3:%s ss4:%s ss5:%s ss6:%s ss7:%s ss8:%s ss9:%s ls0:%s ls1:%s ls2:%s ls3:%s ls4:%s ls5:%s ls6:%s ls7:%s ls8:%s ls9:%s", focus, suspend, pspeed0, pspeed25, pspeed5, pspeed75, pspeed1, pspeed2, pause, espeed0, espeed25, espeed5, espeed75, espeed1, espeed2, god, fly, flyincrease, flydecrease, kg, rt, strang, ss0, ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9, ls0, ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8, ls9);

			fclose(file);

			SetDlgItemTextA(hDlg, IDC_KEYBIND_FOCUS, focus);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SUSPEND, suspend);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_0, pspeed0);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_QUARTER, pspeed25);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_HALF, pspeed5);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_3QUARTERS, pspeed75);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_NORMAL, pspeed1);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_DOUBLE, pspeed2);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_PAUSE, pause);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_0, espeed0);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_QUARTER, espeed25);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_HALF, espeed5);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_3QUARTERS, espeed75);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_NORMAL, espeed1);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_DOUBLE, espeed2);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_GODMODE, god);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_FLYMODE, fly);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_INCREASE_SPEED, flyincrease);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_DECREASE_SPEED, flydecrease);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_KICKGLITCH, kg);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_REACTIONTIME, rt);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_STRANG, strang);

			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_0, ss0);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_1, ss1);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_2, ss2);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_3, ss3);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_4, ss4);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_5, ss5);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_6, ss6);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_7, ss7);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_8, ss8);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_9, ss9);

			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_0, ls0);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_1, ls1);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_2, ls2);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_3, ls3);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_4, ls4);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_5, ls5);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_6, ls6);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_7, ls7);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_8, ls8);
			SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_9, ls9);

			SetTimer(hDlg, 0, 17, NULL);

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE: {
			char focus[0xFF], suspend[0xFF], pspeed0[0xFF], pspeed25[0xFF], pspeed5[0xFF], pspeed75[0xFF], pspeed1[0xFF], pspeed2[0xFF], pause[0xFF], espeed0[0xFF], espeed25[0xFF], espeed5[0xFF], espeed75[0xFF], espeed1[0xFF], espeed2[0xFF], god[0xFF], fly[0xFF], flyincrease[0xFF], flydecrease[0xFF], kg[0xFF], rt[0xFF], strang[0xFF], ss0[0xFF], ss1[0xFF], ss2[0xFF], ss3[0xFF], ss4[0xFF], ss5[0xFF], ss6[0xFF], ss7[0xFF], ss8[0xFF], ss9[0xFF], ls0[0xFF], ls1[0xFF], ls2[0xFF], ls3[0xFF], ls4[0xFF], ls5[0xFF], ls6[0xFF], ls7[0xFF], ls8[0xFF], ls9[0xFF];

			GetDlgItemTextA(hDlg, IDC_KEYBIND_FOCUS, focus, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SUSPEND, suspend, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_0, pspeed0, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_QUARTER, pspeed25, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_HALF, pspeed5, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_3QUARTERS, pspeed75, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_NORMAL, pspeed1, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_DOUBLE, pspeed2, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_PAUSE, pause, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_0, espeed0, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_QUARTER, espeed25, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_HALF, espeed5, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_3QUARTERS, espeed75, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_NORMAL, espeed1, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_DOUBLE, espeed2, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_GODMODE, god, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_FLYMODE, fly, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_INCREASE_SPEED, flyincrease, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_DECREASE_SPEED, flydecrease, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_KICKGLITCH, kg, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_REACTIONTIME, rt, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_STRANG, strang, 0xFF);

			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_0, ss0, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_1, ss1, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_2, ss2, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_3, ss3, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_4, ss4, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_5, ss5, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_6, ss6, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_7, ss7, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_8, ss8, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_9, ss9, 0xFF);

			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_0, ls0, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_1, ls1, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_2, ls2, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_3, ls3, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_4, ls4, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_5, ls5, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_6, ls6, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_7, ls7, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_8, ls8, 0xFF);
			GetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_9, ls9, 0xFF);

			FILE *file = fopen("keybinds.dat", "w");

			fprintf(file, "focus:%s suspend:%s pspeed0:%s pspeed.25:%s pspeed.5:%s pspeed.75:%s pspeed1:%s pspeed2:%s pause:%s espeed0:%s espeed.25:%s espeed.5:%s espeed.75:%s espeed1:%s espeed2:%s god:%s fly:%s flyincrease:%s flydecrease:%s kg:%s rt:%s strang:%s ss0:%s ss1:%s ss2:%s ss3:%s ss4:%s ss5:%s ss6:%s ss7:%s ss8:%s ss9:%s ls0:%s ls1:%s ls2:%s ls3:%s ls4:%s ls5:%s ls6:%s ls7:%s ls8:%s ls9:%s", focus, suspend, pspeed0, pspeed25, pspeed5, pspeed75, pspeed1, pspeed2, pause, espeed0, espeed25, espeed5, espeed75, espeed1, espeed2, god, fly, flyincrease, flydecrease, kg, rt, strang, ss0, ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9, ls0, ls1, ls2, ls3, ls4, ls5, ls6, ls7, ls8, ls9);

			fclose(file);

			EndDialog(hDlg, 0);

			SetupKeybinds();

			// SendMessage(GetDlgItem(hDlg, IDC_KEYBIND_FOCUS), BM_SETSTYLE, (WPARAM)BS_PUSHBUTTON, TRUE);
			SetFocus(NULL);
			return (INT_PTR)TRUE;
		}
		case WM_TIMER:
			if (setBind) {
				for (int keycode = 0; keycode <= 0xFF && setBind; keycode++) {
					if (GetAsyncKeyState(keycode) < 0) {
						for (int i = 0; i < sizeof(KEYS) / sizeof(KEY) && setBind; i++) {
							if (setBind && KEYS[i].keycode == keycode) {
								setBind = false;
								SetDlgItemTextA(hDlg, id, KEYS[i].name);
								id = -1;
							}
						}
					}
				}
			}

			break;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			switch (wmId) {
				case IDC_KEYBIND_FOCUS: case IDC_KEYBIND_SUSPEND: case IDC_KEYBIND_PROCESS_SPEED_0: case IDC_KEYBIND_PROCESS_SPEED_QUARTER: case IDC_KEYBIND_PROCESS_SPEED_HALF: case IDC_KEYBIND_PROCESS_SPEED_3QUARTERS: case IDC_KEYBIND_PROCESS_SPEED_NORMAL: case IDC_KEYBIND_PROCESS_SPEED_DOUBLE: case IDC_KEYBIND_PAUSE: case IDC_KEYBIND_ENGINE_SPEED_0: case IDC_KEYBIND_ENGINE_SPEED_QUARTER: case IDC_KEYBIND_ENGINE_SPEED_HALF: case IDC_KEYBIND_ENGINE_SPEED_3QUARTERS: case IDC_KEYBIND_ENGINE_SPEED_NORMAL: case IDC_KEYBIND_ENGINE_SPEED_DOUBLE: case IDC_KEYBIND_GODMODE: case IDC_KEYBIND_FLYMODE: case IDC_KEYBIND_FLY_INCREASE_SPEED: case IDC_KEYBIND_FLY_DECREASE_SPEED: case IDC_KEYBIND_KICKGLITCH: case IDC_KEYBIND_REACTIONTIME: case IDC_KEYBIND_STRANG: case IDC_KEYBIND_SAVE_SLOT_0: case IDC_KEYBIND_SAVE_SLOT_1: case IDC_KEYBIND_SAVE_SLOT_2: case IDC_KEYBIND_SAVE_SLOT_3: case IDC_KEYBIND_SAVE_SLOT_4: case IDC_KEYBIND_SAVE_SLOT_5: case IDC_KEYBIND_SAVE_SLOT_6: case IDC_KEYBIND_SAVE_SLOT_7: case IDC_KEYBIND_SAVE_SLOT_8: case IDC_KEYBIND_SAVE_SLOT_9: case IDC_KEYBIND_LOAD_SLOT_0: case IDC_KEYBIND_LOAD_SLOT_1: case IDC_KEYBIND_LOAD_SLOT_2: case IDC_KEYBIND_LOAD_SLOT_3: case IDC_KEYBIND_LOAD_SLOT_4: case IDC_KEYBIND_LOAD_SLOT_5: case IDC_KEYBIND_LOAD_SLOT_6: case IDC_KEYBIND_LOAD_SLOT_7: case IDC_KEYBIND_LOAD_SLOT_8: case IDC_KEYBIND_LOAD_SLOT_9:
					id = wmId;
					setBind = true;
					break;
				case IDC_CLEAR_FOCUS:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FOCUS, "none");
					break;
				case IDC_CLEAR_SUSPEND:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SUSPEND, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_0:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_0, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_QUARTER:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_QUARTER, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_HALF:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_HALF, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_3QUARTERS:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_3QUARTERS, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_NORMAL:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_NORMAL, "none");
					break;
				case IDC_CLEAR_PROCESS_SPEED_DOUBLE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PROCESS_SPEED_DOUBLE, "none");
					break;
				case IDC_CLEAR_PAUSE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PAUSE, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_0:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_0, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_QUARTER:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_QUARTER, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_HALF:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_HALF, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_3QUARTERS:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_3QUARTERS, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_NORMAL:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_NORMAL, "none");
					break;
				case IDC_CLEAR_ENGINE_SPEED_DOUBLE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_ENGINE_SPEED_DOUBLE, "none");
					break;
				case IDC_CLEAR_GODMODE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_GODMODE, "none");
					break;
				case IDC_CLEAR_FLYMODE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLYMODE, "none");
					break;
				case IDC_CLEAR_FLY_INCREASE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_INCREASE_SPEED, "none");
					break;
				case IDC_CLEAR_FLY_DECREASE:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_DECREASE_SPEED, "none");
					break;
				case IDC_CLEAR_KICKGLITCH:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_KICKGLITCH, "none");
					break;
				case IDC_CLEAR_REACTIONTIME:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_REACTIONTIME, "none");
					break;
				case IDC_CLEAR_STRANG:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_STRANG, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_0:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_0, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_1:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_1, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_2:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_2, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_3:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_3, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_4:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_4, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_5:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_5, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_6:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_6, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_7:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_7, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_8:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_8, "none");
					break;
				case IDC_CLEAR_SAVE_SLOT_9:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_9, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_0:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_0, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_1:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_1, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_2:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_2, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_3:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_3, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_4:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_4, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_5:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_5, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_6:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_6, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_7:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_7, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_8:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_8, "none");
					break;
				case IDC_CLEAR_LOAD_SLOT_9:
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_9, "none");
					break;
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}