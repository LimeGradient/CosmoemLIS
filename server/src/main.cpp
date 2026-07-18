#include <Windows.h>
#include <MinHook.h>
#include <cstdio>

#include "server/WinCaptureServer.hpp"
#include "server/WinCapture.hpp"
#include "utils/Win32Utils.hpp"

void CreateConsole() {
    AllocConsole();

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$",  "r", stdin);
}

void DestroyConsole() {
    FreeConsole();
}

void OnAttach(HINSTANCE hModule) {
    MH_Initialize();

    CreateConsole();

    Cosmoem::WinCaptureServer::get()->start(2877);
    Cosmoem::WinCapture::get()->setGameHWND(Cosmoem::Utils::Win32Utils::getWindowFromPID(GetCurrentProcessId()));
    Cosmoem::WinCapture::get()->startCapture(Cosmoem::Utils::GameUtils::LIS_BTS_REMASTERED);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OnAttach, 0, 0, 0);
            break;
        case DLL_PROCESS_DETACH:
            MH_DisableHook(MH_ALL_HOOKS);
            MH_Uninitialize();

            break;
        default:
            break;
    }
    return TRUE;
}