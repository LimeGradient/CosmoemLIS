#pragma once

#include "LISRemastered/LISRemasteredSDK.hpp"

namespace LifeIsStrangeRemastered {
    typedef void(__stdcall* fnProcessEvent)(SDK::UObject* thisObj, SDK::UFunction* function, void* params);

    fnProcessEvent GetProcessEventFromVTable();
    void createLISRemasteredChoicesHooks();
}