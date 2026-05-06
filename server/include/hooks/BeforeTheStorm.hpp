#pragma once

#include <Windows.h>

#include "types/Game/BeforeTheStorm.hpp"

namespace BeforeTheStormRemastered {
    typedef DialogChoiceObject*(__stdcall* GetDialogChoice_t)(void* instance, void* methodInfo);
    typedef eChoicePositions(__stdcall* GetPreferredChoice_t)(void* instance, void* methodInfo);
    typedef void(__stdcall* ChoiceMade_t)(void* instance, eInteractMenu button, void* methodInfo);

    void createBTSChoicesHooks(LPVOID* pSetupChoices, ChoiceMade_t oChoiceMade, GetDialogChoice_t oGetDialogChoice, GetPreferredChoice_t oGetPreferredChoice);
}