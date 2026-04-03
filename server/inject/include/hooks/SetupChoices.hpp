#pragma once

#include <Windows.h>

#include "types/Game/BeforeTheStorm.hpp"

typedef DialogChoiceObject*(__stdcall* GetDialogChoice_t)(void* instance, void* methodInfo);

void createChoicesHooks(LPVOID* pSetupChoices, GetDialogChoice_t oGetDialogChoice);