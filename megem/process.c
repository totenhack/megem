DWORD getProcessIdByName(char *name) {
  PROCESSENTRY32 entry = { .dwSize = sizeof(PROCESSENTRY32) };
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (Process32First(snapshot, &entry)) {
    do {
      if (strcmp(entry.szExeFile, name) == 0) {
        CloseHandle(snapshot);
        return entry.th32ProcessID;
      }
    } while (Process32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);

  return -1;
}

int processHasModule(DWORD pid, char *name) {
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

  if (snapshot != INVALID_HANDLE_VALUE) {
    MODULEENTRY32 entry = { .dwSize = sizeof(MODULEENTRY32) };

    if (Module32First(snapshot, &entry)) {
      do {
        if (strcmp(entry.szModule, name) == 0) {
          CloseHandle(snapshot);
          return 1;
        }
      } while (Module32Next(snapshot, &entry));
    }
  }

  CloseHandle(snapshot);
  return 0;
}

DWORD getModuleBaseAddress(DWORD pid, char *name) {
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
  MODULEENTRY32 entry = { .dwSize = sizeof(MODULEENTRY32) };

  if (Module32First(snapshot, &entry)) {
    do {
      if (strcmp(entry.szModule, name) == 0) {
        CloseHandle(snapshot);
        return (DWORD)entry.modBaseAddr;
      }
    } while (Module32Next(snapshot, &entry));
  }

  CloseHandle(snapshot);
  return 0;
}

int injectDll(char *dllPath) {
  char dll[MAX_PATH];

	if (!GetFullPathName(dllPath, MAX_PATH, dll, NULL)) {
		return 0;
	}

	unsigned int dllLen = strlen(dll) + 1;

	LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"),
																					    "LoadLibraryA");

	if (!loadLibAddr) {
		return 0;
	}

	LPVOID memoryAddr = (LPVOID)VirtualAllocEx(process.handle, NULL, dllLen,
																						 MEM_RESERVE | MEM_COMMIT,
																						 PAGE_READWRITE);

	if (!memoryAddr) {
		return 0;
	}

	if (!WriteProcessMemory(process.handle, (LPVOID)memoryAddr, dll, dllLen,
                          NULL))
  {
		return 0;
	}

	if (!CreateRemoteThread(process.handle, NULL, 0,
                          (LPTHREAD_START_ROUTINE)loadLibAddr,
			                    (LPVOID)memoryAddr, 0, NULL))
	{
		return 0;
	}

	CloseHandle(process.handle);
	VirtualFreeEx(process.handle, (LPVOID)memoryAddr, 0, MEM_RELEASE);

  printf("Dll Injected!\n");
  return 1;
}
