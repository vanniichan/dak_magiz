#include <windows.h>

// Hàm Hook Procedure
__declspec(dllexport) LRESULT CALLBACK MaliciousHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    
    // nCode = HC_ACTION nghĩa là có sự kiện hợp lệ
    if (nCode == HC_ACTION) {
        if ((lParam & 0x80000000) == 0) {
            MessageBoxA(NULL, "I can see uuuuu!", "vanld5", MB_OK | MB_ICONWARNING);
        }
    }

    // Gọi hook tiếp theo trong chuỗi để Notepad không bị đơ bàn phím
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}