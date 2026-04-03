#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include "hooks/DX11Hooks.hpp"
#include "hooks/SetupChoices.hpp"
#include "server/Server.hpp"
#include "types/Game/BeforeTheStorm.hpp"

HMODULE DllHandle;

typedef DialogChoiceObject*(__stdcall* GetDialogChoice_t)(void* instance, void* methodInfo);

void installHooks() {
    uintptr_t base = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

    if (!base) {
        printf("Failed to get GameAssembly.dll base!");
        return;
    }

    const uintptr_t DW_SETUP_CHOICES_OFFSET = 0x2F50D0;
    auto pSetupChoices = reinterpret_cast<LPVOID*>(base + DW_SETUP_CHOICES_OFFSET);

    const uintptr_t GET_DIALOG_CHOICE_OFFSET = 0x3208E0;

    createChoicesHooks(pSetupChoices, (GetDialogChoice_t)(base + GET_DIALOG_CHOICE_OFFSET));
    createDX11Hooks();

    printf("Hooks installed successfully!\n");
}

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
    installHooks();

    Server::get()->init(2876);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OnAttach, 0, 0, 0);
            break;
        case DLL_PROCESS_DETACH:
            MH_DisableHook(MH_ALL_HOOKS);
            MH_Uninitialize();

            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();

            break;
        default:
            break;
    }
    return TRUE;
}