#include <Windows.h>
#include <MinHook.h>
#include <iostream>

#include "types/Game/BeforeTheStorm.hpp"

HMODULE DllHandle;

typedef DialogChoiceObject*(__stdcall* GetDialogChoice_t)(void* instance, void* methodInfo);
GetDialogChoice_t oGetDialogChoice = nullptr;

bool(__stdcall* oSetupChoices)(void*, DialogChoiceGraphObject*, void*);
bool hkSetupChoices(void* instance, DialogChoiceGraphObject* dialogChoiceObject, void* methodInfo) {
    printf("setting up choices\n");

    if (!oGetDialogChoice) {
        printf("oGetDialogChoice is null!\n");
        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    if (!dialogChoiceObject) {
        printf("dialogChoiceObject is null!\n");
        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    auto dialogChoice = oGetDialogChoice(dialogChoiceObject , methodInfo);

    uintptr_t dc = (uintptr_t)dialogChoice;
    if (dc < 0x10000 || dc > 0x7FFFFFFFFFFF) {
        printf("dialogChoice is invalid pointer: %p\n", dialogChoice);
        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    Il2CppList* choices = *(Il2CppList**)(dc + 0x1A8);
    if (!choices) {
        printf("choices is null\n");
        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    printf("choices ptr: %p\n", choices);

    uintptr_t cp = (uintptr_t)choices;
    void* items = *(void**)(cp + 0x10);
    int32_t size = *(int32_t*)(cp + 0x18);

    if (!items || size <= 0) {
        printf("items is null or size is 0\n");
        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    printf("choice count: %d\n", choices->size);

    // Read isMajorChoice and autoComplete
    bool isMajorChoice = *(bool*)(dc + 0x1B0);
    bool autoComplete  = *(bool*)(dc + 0x1B1);
    printf("isMajorChoice: %d\n", isMajorChoice);
    printf("autoComplete:  %d\n", autoComplete);

    uintptr_t arrayBase = (uintptr_t)items + 0x20;

    for (int i = 0; i < choices->size; i++) {
        ChoiceObject* choice = *(ChoiceObject**)(arrayBase + i * sizeof(void*));
        if (!choice) continue;

        // Read text directly by raw offset too since struct may be misaligned
        Il2CppString* text = *(Il2CppString**)((uintptr_t)choice + 0x68);
        if (!text) continue;

        printf("choice[%d] text: %s\n", i, ReadIl2CppString(text).c_str());
    }

    return oSetupChoices(instance, dialogChoiceObject, methodInfo);
}

void installHooks() {
    uintptr_t base = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

    if (!base) {
        printf("Failed to get GameAssembly.dll base!");
        return;
    }

    const uintptr_t DW_SETUP_CHOICES_OFFSET = 0x2F50D0;
    auto pSetupChoices = reinterpret_cast<LPVOID*>(base + DW_SETUP_CHOICES_OFFSET);

    const uintptr_t GET_DIALOG_CHOICE_OFFSET = 0x3208E0;
    oGetDialogChoice = (GetDialogChoice_t)(base + GET_DIALOG_CHOICE_OFFSET);

    MH_CreateHook(pSetupChoices, hkSetupChoices, reinterpret_cast<LPVOID*>(&oSetupChoices));
    MH_EnableHook(pSetupChoices);
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
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
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