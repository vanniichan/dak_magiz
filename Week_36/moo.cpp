#define EXPORT_FUNC extern "C" __declspec(dllexport)

EXPORT_FUNC int GetHookAPIs(PVOID a, PVOID b, PVOID c)
{
    return 0x01; 
}

EXPORT_FUNC int NotifyShims(PVOID a, PVOID b)
{
    return 0x01; 
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH) {
        return TRUE;
    }
    return TRUE;
}