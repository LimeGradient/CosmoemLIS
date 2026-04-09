#include <Windows.h>
#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include "Log.hpp"
#include "hooks/DX11Hooks.hpp"
#include "hooks/SetupChoices.hpp"
#include "server/Server.hpp"
#include "types/Game/BeforeTheStorm.hpp"

HMODULE DllHandle;

typedef DialogChoiceObject*(__stdcall* GetDialogChoice_t)(void* instance, void* methodInfo);

void installHooks() {
    uintptr_t base = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

    if (!base) {
        Logging::error("Failed to get GameAssembly.dll base");
        return;
    }

    const uintptr_t DW_SETUP_CHOICES_OFFSET = 0x2F50D0;
    auto pSetupChoices = reinterpret_cast<LPVOID*>(base + DW_SETUP_CHOICES_OFFSET);

    const uintptr_t CHOICE_MADE_OFFSET = 0x2F35B0;
    const uintptr_t GET_DIALOG_CHOICE_OFFSET = 0x3208E0;
    const uintptr_t GET_PREFERRED_CHOICE_OFFSET = 0x24E580;

    createChoicesHooks(pSetupChoices, (ChoiceMade_t)(base + CHOICE_MADE_OFFSET), (GetDialogChoice_t)(base + GET_DIALOG_CHOICE_OFFSET), (GetPreferredChoice_t)(base + GET_PREFERRED_CHOICE_OFFSET));
    createDX11Hooks();

    Logging::info("Hooks installed successfully");
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