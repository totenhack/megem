#include "stdafx.h"

#define SetTextFromKey(id, key) if (!key) { SetDlgItemTextA(hDlg, id, "None"); } else { GetKeyNameTextA(MapVirtualKey(key, MAPVK_VK_TO_VSC) << 16, text, sizeof(text)); SetDlgItemTextA(hDlg, id, text); }
WNDPROC visualPropertyProc;

PromptProc(VisualPropertiesProc) {
	static HWND child;
	HWND list = GetDlgItem(hDlg, IDC_LIST);

	switch (message) {
		case WM_INITDIALOG: {
			LVCOLUMN lvc;

			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 150;
			lvc.pszText = TEXT("Name");
			lvc.cchTextMax = 0xFF;

			ListView_InsertColumn(list, 0, &lvc);

			lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			lvc.cx = 158;
			lvc.pszText = TEXT("Value");

			ListView_InsertColumn(list, 1, &lvc);

			TCHAR buffer[0xFF];

			LVITEM item;
			item.mask = LVIF_TEXT;
			item.cchTextMax = 0xFF;
			item.iItem = 0;
			item.iSubItem = 0;
			item.pszText = buffer;

			for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
				MultiByteToWideChar(CP_ACP, 0, visual_properties[i].name, -1, buffer, 0xFF);
				ListView_InsertItem(list, &item);

				if (GetProcess()) {
					char c[0xFF];

					DWORD base = i < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
					if (visual_properties[i].type == TYPE_FLOAT) {
						float value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%f", value);
						if (!GetSettings()->toggle_vp) {
							GetSettings()->visual_properties[i] = *(DWORD *)&value;
						}
					} else {
						int value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%d", value);
						if (!GetSettings()->toggle_vp) {
							GetSettings()->visual_properties[i] = *(DWORD *)&value;
						}
					}

					MultiByteToWideChar(CP_ACP, 0, c, -1, buffer, 0xFF);
				} else {
					wcscpy(buffer, TEXT("0"));
				}

				ListView_SetItemText(list, 0, 1, buffer);
			}

			SetTimer(hDlg, 0, 5000, NULL);

			return (INT_PTR)TRUE;
		}
		case WM_TIMER: {
			TCHAR buffer[0xFF];

			for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
				if (GetProcess()) {
					char c[0xFF];

					DWORD base = i < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
					if (visual_properties[i].type == TYPE_FLOAT) {
						float value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%f", value);
					} else {
						int value;
						ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[i].offset), &value, 4, NULL);
						sprintf(c, "%d", value);
					}

					MultiByteToWideChar(CP_ACP, 0, c, -1, buffer, 0xFF);
				} else {
					wcscpy(buffer, TEXT("0"));
				}

				ListView_SetItemText(list, i, 1, buffer);
			}

			break;
		}
		case WM_CLOSE: {
			char text[0xFF];
			GetTempPathA(0xFF, text);
			strcat(text, "megem.settings");
			FILE *file = fopen(text, "wb");
			if (file) {
				fwrite(GetSettings(), sizeof(SETTINGS), 1, file);
				fclose(file);
			}
			DestroyWindow(child);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}
		case WM_NOTIFY: {
			switch (((LPNMHDR)lParam)->code) {
				case NM_DBLCLK: {
					int index = ListView_GetNextItem(list, -1, LVNI_SELECTED);

					if (index >= 0) {
						DestroyWindow(child);

						char c[0xFF];

						strcpy(c, visual_properties[index].name);
						child = CreateWindowA("EDIT", c, WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 200, 65, hDlg, 0, GetModuleHandle(0), NULL);

						if (GetProcess()) {
							DWORD base = index < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
							if (visual_properties[index].type == TYPE_FLOAT) {
								float value;
								ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[index].offset), &value, 4, NULL);
								sprintf(c, "%f", value);
							} else {
								int value;
								ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[index].offset), &value, 4, NULL);
								sprintf(c, "%d", value);
							}
						} else {
							strcpy(c, "0");
						}

						SetWindowTextA(child, c);
						visualPropertyProc = (WNDPROC)SetWindowLong(child, GWL_WNDPROC, (LONG)VisualPropertyProc);
						VisualPropertyProc(child, -1, index, index);
					}

					break;
				}
			}

			break;
		}
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);

			switch (wmId) {
				case IDC_SAVE: {
					TCHAR tpath[MAX_PATH] = { 0 };

					OPENFILENAME name;
					memset(&name, 0, sizeof(OPENFILENAME));
					name.lStructSize = sizeof(OPENFILENAME);
					name.hwndOwner = NULL;
					name.lpstrFilter = TEXT("*.vprops");
					name.lpstrFile = tpath;
					name.nMaxFile = MAX_PATH;
					name.Flags = OFN_OVERWRITEPROMPT;
					name.lpstrDefExt = TEXT("vprops");

					if (GetSaveFileName(&name)) {
						char path[MAX_PATH];
						wcstombs(path, tpath, MAX_PATH);

						FILE *file = fopen(path, "wb");

						int value = 0;
						for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
							if (GetProcess()) {
								DWORD base = i < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
								ReadProcessMemory(GetProcess(), (LPCVOID)(base + visual_properties[i].offset), &value, 4, NULL);
							} else {
								value = 0;
							}

							fwrite(&value, sizeof(int), 1, file);
						}

						fclose(file);
					}

					break;
				}
				case IDC_LOAD: {
					TCHAR tpath[MAX_PATH] = { 0 };

					OPENFILENAME name;
					memset(&name, 0, sizeof(OPENFILENAME));
					name.lStructSize = sizeof(OPENFILENAME);
					name.hwndOwner = NULL;
					name.lpstrFilter = TEXT("*.vprops");
					name.lpstrFile = tpath;
					name.nMaxFile = MAX_PATH;
					name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					name.lpstrDefExt = TEXT("vprops");

					if (GetOpenFileName(&name)) {
						char path[MAX_PATH];
						wcstombs(path, tpath, MAX_PATH);

						FILE *file = fopen(path, "rb");

						int value = 0;
						for (int i = LENGTH(visual_properties) - 1; i > -1; i--) {
							fread(&value, sizeof(int), 1, file);

							if (GetProcess()) {
								DWORD base = i < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
								WriteProcessMemory(GetProcess(), (LPVOID)(base + visual_properties[i].offset), &value, 4, NULL);
								GetSettings()->visual_properties[i] = *(DWORD *)&value;
							}
						}

						fclose(file);
					}

					SendMessage(hDlg, WM_TIMER, 0, 0);

					break;
				}
			}

			break;
		}
	}

	return (INT_PTR)FALSE;
}

PromptProc(VisualPropertyProc) {
	static int index;

	switch (message) {
		case -1:
			if (wParam == lParam) {
				index = wParam;
			}

			break;
		case WM_KEYDOWN: {
			if (wParam == VK_RETURN) {
				char c[0xFF];

				GetWindowTextA(hDlg, c, 0xFF);
				DWORD base = index < PROPERTIES_ENGINE_START ? GetData().dof_base : GetData().engine_base;
				if (visual_properties[index].type == TYPE_FLOAT) {
					float value = (float)atof(c);
					WriteProcessMemory(GetProcess(), (LPVOID)(base + visual_properties[index].offset), &value, 4, NULL);
					GetSettings()->visual_properties[index] = *(DWORD *)&value;
				} else {
					int value = atoi(c);
					WriteProcessMemory(GetProcess(), (LPVOID)(base + visual_properties[index].offset), &value, 4, NULL);
					GetSettings()->visual_properties[index] = *(DWORD *)&value;
				}

				SendMessage(GetWindow(hDlg, GW_OWNER), WM_TIMER, 0, 0);

				return DestroyWindow(hDlg);
			}

			break;
		}
	}

	return CallWindowProc(visualPropertyProc, hDlg, message, wParam, lParam);
}

void swap(char *src, char *dest) {
	char t = *src;
	*src = *dest;
	*dest = t;
}

PromptProc(SettingsProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%06x", GetSettings()->color_background);
			swap(text, text + 4);
			swap(text + 1, text + 5);
			SetDlgItemTextA(hDlg, IDC_BG_COLOR, text);

			sprintf(text, "%06x", GetSettings()->color_text);
			swap(text, text + 4);
			swap(text + 1, text + 5);
			SetDlgItemTextA(hDlg, IDC_TEXT_COLOR, text);

			sprintf(text, "%06x", GetSettings()->color_active);
			swap(text, text + 4);
			swap(text + 1, text + 5);
			SetDlgItemTextA(hDlg, IDC_ACTIVE_COLOR, text);

			if (GetSettings()->toggle_fov) {
				CheckDlgButton(hDlg, IDC_CHECK_FOV, BST_CHECKED);
			}

			if (GetSettings()->toggle_vp) {
				CheckDlgButton(hDlg, IDC_CHECK_VP, BST_CHECKED);
			}

			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_CHECK_FOV:
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_FOV)) {
						GetSettings()->toggle_fov = true;
					} else {
						GetSettings()->toggle_fov = false;
					}
					break;
				case IDC_CHECK_VP:
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_VP)) {
						GetSettings()->toggle_vp = true;
					} else {
						GetSettings()->toggle_vp = false;
					}
					break;
			}
			return (INT_PTR)TRUE;
		}
		case WM_CLOSE: {
			char text[0xFF];
			int r, g, b;

			GetDlgItemTextA(hDlg, IDC_BG_COLOR, text, 0xFF);
			r = g = b = 0;
			sscanf(text, "%2x%2x%2x", &r, &g, &b);
			GetSettings()->color_background = RGB(r & 0xFF, g & 0xFF, b & 0xFF);

			GetDlgItemTextA(hDlg, IDC_TEXT_COLOR, text, 0xFF);
			r = g = b = 0;
			sscanf(text, "%2x%2x%2x", &r, &g, &b);
			GetSettings()->color_text = RGB(r & 0xFF, g & 0xFF, b & 0xFF);

			GetDlgItemTextA(hDlg, IDC_ACTIVE_COLOR, text, 0xFF);
			r = g = b = 0;
			sscanf(text, "%2x%2x%2x", &r, &g, &b);
			GetSettings()->color_active = RGB(r & 0xFF, g & 0xFF, b & 0xFF);

			GetTempPathA(0xFF, text);
			strcat(text, "megem.settings");
			FILE *file = fopen(text, "wb");
			if (file) {
				fwrite(GetSettings(), sizeof(SETTINGS), 1, file);
				fclose(file);
			}

			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}
	}

	return (INT_PTR)FALSE;
}

PromptProc(DollyCameraProc) {
	switch (message) {
		case WM_INITDIALOG: {
			if (GetProcess()) {
				CallFunction("EXPORT_ResetDolly", 0);
			}
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_ADDMARKER: {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_FRAME, text, 0xFF);

					DWORD speed = (DWORD)atoi(text);

					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(DWORD), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(DWORD), NULL);
					WaitForSingleObject(CallFunction("EXPORT_AddDollyMarker", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
					break;
				}
				case IDC_PLAY:
					CallFunction("EXPORT_PlayDolly", 0);
					break;
				case IDC_RESET:
					CallFunction("EXPORT_ResetDolly", 0);
					break;
			}
			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(KeybindsProc) {
	static char text[0xFF];
	static DWORD active_id = 0;

	switch (message) {
		case WM_INITDIALOG: {
			KEYBINDS *keybinds = GetKeybinds();
			
			SetTextFromKey(IDC_KEYBIND_PAUSE, keybinds->pause);
			SetTextFromKey(IDC_KEYBIND_SPEED_0, keybinds->speed0);
			SetTextFromKey(IDC_KEYBIND_SPEED_25, keybinds->speed25);
			SetTextFromKey(IDC_KEYBIND_SPEED_5, keybinds->speed5);
			SetTextFromKey(IDC_KEYBIND_SPEED_75, keybinds->speed75);
			SetTextFromKey(IDC_KEYBIND_SPEED_1, keybinds->speed1);
			SetTextFromKey(IDC_KEYBIND_SPEED_2, keybinds->speed2);
			SetTextFromKey(IDC_KEYBIND_GOD, keybinds->god);
			SetTextFromKey(IDC_KEYBIND_FLY, keybinds->fly);
			SetTextFromKey(IDC_KEYBIND_FLY_INCREASE, keybinds->fly_increase);
			SetTextFromKey(IDC_KEYBIND_FLY_DECREASE, keybinds->fly_decrease);
			SetTextFromKey(IDC_KEYBIND_FLY_LEFT, keybinds->fly_left);
			SetTextFromKey(IDC_KEYBIND_FLY_RIGHT, keybinds->fly_right);
			SetTextFromKey(IDC_KEYBIND_FLY_FORWARD, keybinds->fly_forward);
			SetTextFromKey(IDC_KEYBIND_FLY_BACKWARD, keybinds->fly_backward);
			SetTextFromKey(IDC_KEYBIND_KICKGLITCH, keybinds->kickglitch);
			SetTextFromKey(IDC_KEYBIND_REACTIONTIME, keybinds->reactiontime);
			SetTextFromKey(IDC_KEYBIND_STRANG, keybinds->strang);
			SetTextFromKey(IDC_KEYBIND_BEAMER, keybinds->beamer);
			SetTextFromKey(IDC_KEYBIND_QUICK_TURN, keybinds->quickturn);
			SetTextFromKey(IDC_KEYBIND_CHECKPOINT, keybinds->checkpoint);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_0, keybinds->s0);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_1, keybinds->s1);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_2, keybinds->s2);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_3, keybinds->s3);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_4, keybinds->s4);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_5, keybinds->s5);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_6, keybinds->s6);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_7, keybinds->s7);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_8, keybinds->s8);
			SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_9, keybinds->s9);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_0, keybinds->l0);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_1, keybinds->l1);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_2, keybinds->l2);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_3, keybinds->l3);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_4, keybinds->l4);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_5, keybinds->l5);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_6, keybinds->l6);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_7, keybinds->l7);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_8, keybinds->l8);
			SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_9, keybinds->l9);

			SetTimer(hDlg, 0, 17, NULL);

			return (INT_PTR)TRUE;
		}
		case WM_TIMER: {
			if (active_id) {
				short key;
				for (key = 4; key <= 0xFF; ++key) {
					if (IsKeyDown(key)) {
						switch (active_id) {
							case IDC_KEYBIND_PAUSE:
								GetKeybinds()->pause = key;
								SetTextFromKey(IDC_KEYBIND_PAUSE, key);
								break;
							case IDC_KEYBIND_SPEED_0:
								GetKeybinds()->speed0 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_0, key);
								break;
							case IDC_KEYBIND_SPEED_25:
								GetKeybinds()->speed25 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_25, key);
								break;
							case IDC_KEYBIND_SPEED_5:
								GetKeybinds()->speed25 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_5, key);
								break;
							case IDC_KEYBIND_SPEED_75:
								GetKeybinds()->speed75 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_75, key);
								break;
							case IDC_KEYBIND_SPEED_1:
								GetKeybinds()->speed1 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_1, key);
								break;
							case IDC_KEYBIND_SPEED_2:
								GetKeybinds()->speed2 = key;
								SetTextFromKey(IDC_KEYBIND_SPEED_2, key);
								break;
							case IDC_KEYBIND_GOD:
								GetKeybinds()->god = key;
								SetTextFromKey(IDC_KEYBIND_GOD, key);
								break;
							case IDC_KEYBIND_FLY:
								GetKeybinds()->fly = key;
								SetTextFromKey(IDC_KEYBIND_FLY, key);
								break;
							case IDC_KEYBIND_FLY_INCREASE:
								GetKeybinds()->fly_increase = key;
								SetTextFromKey(IDC_KEYBIND_FLY_INCREASE, key);
								break;
							case IDC_KEYBIND_FLY_DECREASE:
								GetKeybinds()->fly_decrease = key;
								SetTextFromKey(IDC_KEYBIND_FLY_DECREASE, key);
								break;
							case IDC_KEYBIND_FLY_LEFT:
								GetKeybinds()->fly_left = key;
								SetTextFromKey(IDC_KEYBIND_FLY_LEFT, key);
								break;
							case IDC_KEYBIND_FLY_RIGHT:
								GetKeybinds()->fly_right = key;
								SetTextFromKey(IDC_KEYBIND_FLY_RIGHT, key);
								break;
							case IDC_KEYBIND_FLY_FORWARD:
								GetKeybinds()->fly_forward = key;
								SetTextFromKey(IDC_KEYBIND_FLY_FORWARD, key);
								break;
							case IDC_KEYBIND_FLY_BACKWARD:
								GetKeybinds()->fly_backward = key;
								SetTextFromKey(IDC_KEYBIND_FLY_BACKWARD, key);
								break;
							case IDC_KEYBIND_KICKGLITCH:
								GetKeybinds()->kickglitch = key;
								SetTextFromKey(IDC_KEYBIND_KICKGLITCH, key);
								break;
							case IDC_KEYBIND_REACTIONTIME:
								GetKeybinds()->reactiontime = key;
								SetTextFromKey(IDC_KEYBIND_REACTIONTIME, key);
								break;
							case IDC_KEYBIND_STRANG:
								GetKeybinds()->strang = key;
								SetTextFromKey(IDC_KEYBIND_STRANG, key);
								break;
							case IDC_KEYBIND_BEAMER:
								GetKeybinds()->beamer = key;
								SetTextFromKey(IDC_KEYBIND_BEAMER, key);
								break;
							case IDC_KEYBIND_QUICK_TURN:
								GetKeybinds()->quickturn = key;
								SetTextFromKey(IDC_KEYBIND_QUICK_TURN, key);
								break;
							case IDC_KEYBIND_CHECKPOINT:
								GetKeybinds()->checkpoint = key;
								SetTextFromKey(IDC_KEYBIND_CHECKPOINT, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_0:
								GetKeybinds()->s0 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_0, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_1:
								GetKeybinds()->s1 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_1, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_2:
								GetKeybinds()->s2 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_2, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_3:
								GetKeybinds()->s3 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_3, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_4:
								GetKeybinds()->s4 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_4, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_5:
								GetKeybinds()->s5 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_5, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_6:
								GetKeybinds()->s6 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_6, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_7:
								GetKeybinds()->s7 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_7, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_8:
								GetKeybinds()->s8 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_8, key);
								break;
							case IDC_KEYBIND_SAVE_SLOT_9:
								GetKeybinds()->s9 = key;
								SetTextFromKey(IDC_KEYBIND_SAVE_SLOT_9, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_0:
								GetKeybinds()->l0 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_0, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_1:
								GetKeybinds()->l1 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_1, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_2:
								GetKeybinds()->l2 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_2, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_3:
								GetKeybinds()->l3 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_3, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_4:
								GetKeybinds()->l4 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_4, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_5:
								GetKeybinds()->l5 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_5, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_6:
								GetKeybinds()->l6 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_6, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_7:
								GetKeybinds()->l7 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_7, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_8:
								GetKeybinds()->l8 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_8, key);
								break;
							case IDC_KEYBIND_LOAD_SLOT_9:
								GetKeybinds()->l9 = key;
								SetTextFromKey(IDC_KEYBIND_LOAD_SLOT_9, key);
								break;
						}
						active_id = 0;
						break;
					}
				}
			}

			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			DWORD id = LOWORD(wParam);
			switch (id) {
				case IDC_KEYBIND_PAUSE:case IDC_KEYBIND_SPEED_0:case IDC_KEYBIND_SPEED_25:case IDC_KEYBIND_SPEED_5:case IDC_KEYBIND_SPEED_75:case IDC_KEYBIND_SPEED_1:case IDC_KEYBIND_SPEED_2:case IDC_KEYBIND_GOD:case IDC_KEYBIND_FLY:case IDC_KEYBIND_FLY_INCREASE:case IDC_KEYBIND_FLY_DECREASE:case IDC_KEYBIND_FLY_LEFT:case IDC_KEYBIND_FLY_RIGHT:case IDC_KEYBIND_FLY_FORWARD:case IDC_KEYBIND_FLY_BACKWARD:case IDC_KEYBIND_KICKGLITCH:case IDC_KEYBIND_REACTIONTIME:case IDC_KEYBIND_STRANG:case IDC_KEYBIND_BEAMER:case IDC_KEYBIND_QUICK_TURN:case IDC_KEYBIND_CHECKPOINT:case IDC_KEYBIND_SAVE_SLOT_0:case IDC_KEYBIND_SAVE_SLOT_1:case IDC_KEYBIND_SAVE_SLOT_2:case IDC_KEYBIND_SAVE_SLOT_3:case IDC_KEYBIND_SAVE_SLOT_4:case IDC_KEYBIND_SAVE_SLOT_5:case IDC_KEYBIND_SAVE_SLOT_6:case IDC_KEYBIND_SAVE_SLOT_7:case IDC_KEYBIND_SAVE_SLOT_8:case IDC_KEYBIND_SAVE_SLOT_9:case IDC_KEYBIND_LOAD_SLOT_0:case IDC_KEYBIND_LOAD_SLOT_1:case IDC_KEYBIND_LOAD_SLOT_2:case IDC_KEYBIND_LOAD_SLOT_3:case IDC_KEYBIND_LOAD_SLOT_4:case IDC_KEYBIND_LOAD_SLOT_5:case IDC_KEYBIND_LOAD_SLOT_6:case IDC_KEYBIND_LOAD_SLOT_7:case IDC_KEYBIND_LOAD_SLOT_8:case IDC_KEYBIND_LOAD_SLOT_9:
					active_id = id;
					break;
				case IDC_CLEAR_PAUSE:
					GetKeybinds()->pause = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_PAUSE, "None");
					break;
				case IDC_CLEAR_SPEED_0:
					GetKeybinds()->speed0 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_0, "None");
					break;
				case IDC_CLEAR_SPEED_25:
					GetKeybinds()->speed25 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_25, "None");
					break;
				case IDC_CLEAR_SPEED_5:
					GetKeybinds()->speed5 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_5, "None");
					break;
				case IDC_CLEAR_SPEED_75:
					GetKeybinds()->speed75 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_75, "None");
					break;
				case IDC_CLEAR_SPEED_1:
					GetKeybinds()->speed1 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_1, "None");
					break;
				case IDC_CLEAR_SPEED_2:
					GetKeybinds()->speed2 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SPEED_2, "None");
					break;
				case IDC_CLEAR_GOD:
					GetKeybinds()->god = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_GOD, "None");
					break;
				case IDC_CLEAR_FLY:
					GetKeybinds()->fly = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY, "None");
					break;
				case IDC_CLEAR_FLY_INCREASE:
					GetKeybinds()->fly_increase = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_INCREASE, "None");
					break;
				case IDC_CLEAR_FLY_DECREASE:
					GetKeybinds()->fly_decrease = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_DECREASE, "None");
					break;
				case IDC_CLEAR_FLY_LEFT:
					GetKeybinds()->fly_left = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_LEFT, "None");
					break;
				case IDC_CLEAR_FLY_RIGHT:
					GetKeybinds()->fly_right = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_RIGHT, "None");
					break;
				case IDC_CLEAR_FLY_FORWARD:
					GetKeybinds()->fly_forward = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_FORWARD, "None");
					break;
				case IDC_CLEAR_FLY_BACKWARD:
					GetKeybinds()->fly_backward = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_FLY_BACKWARD, "None");
					break;
				case IDC_CLEAR_KICKGLITCH:
					GetKeybinds()->kickglitch = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_KICKGLITCH, "None");
					break;
				case IDC_CLEAR_REACTIONTIME:
					GetKeybinds()->reactiontime = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_REACTIONTIME, "None");
					break;
				case IDC_CLEAR_STRANG:
					GetKeybinds()->strang = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_STRANG, "None");
					break;
				case IDC_CLEAR_BEAMER:
					GetKeybinds()->beamer = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_BEAMER, "None");
					break;
				case IDC_CLEAR_QUICK_TURN:
					GetKeybinds()->quickturn = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_QUICK_TURN, "None");
					break;
				case IDC_CLEAR_CHECKPOINT:
					GetKeybinds()->checkpoint = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_CHECKPOINT, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_0:
					GetKeybinds()->s0 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_0, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_1:
					GetKeybinds()->s1 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_1, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_2:
					GetKeybinds()->s2 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_2, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_3:
					GetKeybinds()->s3 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_3, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_4:
					GetKeybinds()->s4 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_4, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_5:
					GetKeybinds()->s5 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_5, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_6:
					GetKeybinds()->s6 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_6, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_7:
					GetKeybinds()->s7 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_7, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_8:
					GetKeybinds()->s8 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_8, "None");
					break;
				case IDC_CLEAR_SAVE_SLOT_9:
					GetKeybinds()->s9 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_SAVE_SLOT_9, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_0:
					GetKeybinds()->l0 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_0, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_1:
					GetKeybinds()->l1 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_1, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_2:
					GetKeybinds()->l2 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_2, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_3:
					GetKeybinds()->l3 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_3, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_4:
					GetKeybinds()->l4 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_4, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_5:
					GetKeybinds()->l5 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_5, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_6:
					GetKeybinds()->l6 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_6, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_7:
					GetKeybinds()->l7 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_7, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_8:
					GetKeybinds()->l8 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_8, "None");
					break;
				case IDC_CLEAR_LOAD_SLOT_9:
					GetKeybinds()->l9 = 0;
					SetDlgItemTextA(hDlg, IDC_KEYBIND_LOAD_SLOT_9, "None");
					break;
			}

			return (INT_PTR)TRUE;
		}
		case WM_CLOSE: {
			char path[0xFF] = { 0 };
			GetTempPathA(sizeof(path), path);
			strcat(path, "megem.keybinds");
			FILE *file = fopen(path, "wb");
			if (file) {
				fwrite(GetKeybinds(), sizeof(KEYBINDS), 1, file);
				fclose(file);
			}
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}
	}

	return (INT_PTR)FALSE;
}

PromptProc(LevelStreamerProc) {
	HWND list = GetDlgItem(hDlg, IDC_LEVELLIST);
	switch (message) {
		case WM_INITDIALOG:
			SetTimer(hDlg, 0, 5000, NULL);
		case WM_TIMER: {
			int top = SendMessage(list, LB_GETTOPINDEX, 0, 0);
			SendMessage(list, LB_RESETCONTENT, 0, 0);
			if (GetProcess()) {
				WaitForSingleObject(CallFunction("EXPORT_GetSublevels", 0), INFINITE);
				char str[0xFF];
				wchar_t wstr[0xFF];
				LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 0xFF, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				for (int i = 0; i < GetData().sublevel_count; ++i) {
					ReadBuffer(GetProcess(), (char *)(GetData().sublevels + (i * 0xFF)), str, 0xFF);
					WriteBuffer(GetProcess(), arg, str, 0xFF);
					WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);
					if (ReadChar(GetProcess(), arg) & 1) {
						prepend(str, "Loaded - ");
					} else {
						prepend(str, "Unloaded - ");
					}

					CharToWChar(wstr, str);
					SendMessage(list, LB_ADDSTRING, 0, (LPARAM)wstr);
				}
				VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
			}
			SendMessage(list, LB_SETTOPINDEX, top, 0);

			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_LEVELLIST: {
					switch (HIWORD(wParam)) {
						case LBN_DBLCLK: {
							wchar_t wstr[0xFF];
							char str[0xFF];

							SendMessage(list, LB_GETTEXT, (WPARAM)(int)SendMessage(list, LB_GETCURSEL, 0, 0), (LPARAM)wstr);
							wcstombs(str, wstr, 0xFF);

							bool load = !(_strnicmp(str, "Loaded", 6) == 0);
							memmove(str, strchr(str, '-') + 2, 0xFF);

							LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 0xFFF, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
							WriteProcessMemory(GetProcess(), arg, str, 0xFFF, NULL);

							if (load) {
								WaitForSingleObject(CallFunction("EXPORT_LoadLevelStreamByString", arg), INFINITE);

								char status = 0;
								while (!(status & 1)) {
									WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);
									ReadProcessMemory(GetProcess(), arg, &status, 1, NULL);
									WriteProcessMemory(GetProcess(), arg, str, 1, NULL);

									Sleep(100);
								}
							} else {
								WaitForSingleObject(CallFunction("EXPORT_UnloadLevelStreamByString", arg), INFINITE);

								char status = 1;
								while (status & 1) {
									WaitForSingleObject(CallFunction("EXPORT_GetSublevelStatus", arg), INFINITE);
									ReadProcessMemory(GetProcess(), arg, &status, 1, NULL);
									WriteProcessMemory(GetProcess(), arg, str, 1, NULL);

									Sleep(100);
								}
							}

							VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
							LevelStreamerProc(hDlg, WM_TIMER, 0, 0);
						}
					}
					break;
				}
				case IDC_LOADALL: case IDC_UNLOADALL: {
					if (GetProcess()) {
						WaitForSingleObject(CallFunction("EXPORT_GetSublevels", 0), INFINITE);
						char str[0xFF];
						LPVOID arg = VirtualAllocEx(GetProcess(), NULL, 0xFF, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
						for (int i = 0; i < GetData().sublevel_count; ++i) {
							ReadBuffer(GetProcess(), (char *)(GetData().sublevels + (i * 0xFF)), str, 0xFF);
							WriteBuffer(GetProcess(), arg, str, 0xFF);
							if (LOWORD(wParam) == IDC_LOADALL) {
								WaitForSingleObject(CallFunction("EXPORT_LoadLevelStreamByString", arg), INFINITE);
							} else {
								WaitForSingleObject(CallFunction("EXPORT_UnloadLevelStreamByString", arg), INFINITE);
							}
						}
						VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
					}
				}
			}
			return (INT_PTR)TRUE;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(FOVProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData().fov);
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float fov = (float)atof(text);
					GetSettings()->fov = fov;

					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &fov, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetFOV", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
			} else break;
		}
		case WM_CLOSE: {
			char text[0xFF];
			GetTempPathA(0xFF, text);
			strcat(text, "megem.settings");
			FILE *file = fopen(text, "wb");
			if (file) {
				fwrite(GetSettings(), sizeof(SETTINGS), 1, file);
				fclose(file);
			}
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}
	}

	return (INT_PTR)FALSE;
}

PromptProc(ProcessSpeedProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			sprintf(text, "%f", GetData().process_speed);
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					float speed = (float)atof(text);

					LPVOID arg = VirtualAllocEx(GetProcess(), NULL, sizeof(float), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
					WriteProcessMemory(GetProcess(), arg, &speed, sizeof(float), NULL);
					WaitForSingleObject(CallFunction("EXPORT_SetProcessSpeed", arg), INFINITE);
					VirtualFreeEx(GetProcess(), arg, 0, MEM_RELEASE);
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(EngineSpeedProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().engine_base + ENGINE_SPEED), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerXProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_X), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerYProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Y), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerZProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Z)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_Z), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerVXProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VX)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VX), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerVYProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VY)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VY), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerVZProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VZ)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_VZ), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerRXProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", IntToDegrees(ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RX))));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RX), DegreesToInt((float)atof(text)));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerRYProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", IntToDegrees(ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY))));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RY), DegreesToInt((float)atof(text)));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerRZProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", IntToDegrees(ReadInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RZ))));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteInt(GetProcess(), (void *)(GetData().camera_base + CAMERA_RZ), DegreesToInt((float)atof(text)));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerScaleProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SCALE)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SCALE), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerSXProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SX)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SX), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerSYProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SY)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SY), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

PromptProc(PlayerSZProc) {
	switch (message) {
		case WM_INITDIALOG: {
			char text[0xFF];
			if (GetProcess()) {
				sprintf(text, "%f", ReadFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SZ)));
			} else {
				sprintf(text, "%f", (float)0);
			}
			SetDlgItemTextA(hDlg, IDC_INPUT, text);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if (LOWORD(wParam) == IDC_OK) {
				if (GetProcess()) {
					char text[0xFF];
					GetDlgItemTextA(hDlg, IDC_INPUT, text, 0xFF);

					WriteFloat(GetProcess(), (void *)(GetData().player_base + PLAYER_SZ), (float)atof(text));
				}
			} else break;
		}
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}