#include "spoof.h"
#include <windows.h>

namespace hooks {
    void init_hooks();
}

// Injector'un aradigi export
extern "C" __declspec(dllexport) int sfsqofsjqjfsqdnsqnfdsqoif(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
#ifndef DEV
        HMODULE handle = NULL;
        BOOL ret = SPOOF_CALL(GetModuleHandleExW)(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, (LPCWSTR)DllMain, &handle);
        if (!ret)
            return FALSE;
#endif
        SPOOF_CALL(DisableThreadLibraryCalls)(hModule);
        hooks::init_hooks();
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
