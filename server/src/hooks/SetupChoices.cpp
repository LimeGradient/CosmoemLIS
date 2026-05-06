#include "hooks/SetupChoices.hpp"

#include <atomic>
#include <MinHook.h>

#include "Log.hpp"
#include "packets/Server.hpp"
#include "panel/VotesPanel.hpp"
#include "server/Server.hpp"
#include "server/GameManager.hpp"

std::atomic<bool> timerSpawned = false;

namespace BeforeTheStormRemastered {
    bool(__stdcall* oSetupChoices)(void*, DialogChoiceGraphObject*, void*);
    bool hkSetupChoices(void* instance, DialogChoiceGraphObject* dialogChoiceObject, void* methodInfo);

    ChoiceMade_t oChoiceMade = nullptr;
    GetDialogChoice_t oGetDialogChoice = nullptr;
    GetPreferredChoice_t oGetPreferredChoice = nullptr;

    bool hkSetupChoices(void* instance, DialogChoiceGraphObject* dialogChoiceObject, void* methodInfo) {
        if (!oGetDialogChoice) {
            Logging::error("oGetDialogChoice is null");
            return oSetupChoices(instance, dialogChoiceObject, methodInfo);
        }

        if (!dialogChoiceObject) {
            Logging::error("dialogChoiceObject is null");
            return oSetupChoices(instance, dialogChoiceObject, methodInfo);
        }

        auto dialogChoice = oGetDialogChoice(dialogChoiceObject , methodInfo);

        uintptr_t dc = (uintptr_t)dialogChoice;
        if (dc < 0x10000 || dc > 0x7FFFFFFFFFFF) {
            Logging::error("dialogChoice is an invalid pointer");
            return oSetupChoices(instance, dialogChoiceObject, methodInfo);
        }

        Il2CppList* choices = *(Il2CppList**)(dc + 0x1A8);
        if (!choices) {
            Logging::error("Choices is an invalid pointer: {}", (void*)choices);
            return oSetupChoices(instance, dialogChoiceObject, methodInfo);
        }

        Logging::info("Choices ptr: {}", (void*)choices);

        uintptr_t cp = (uintptr_t)choices;
        void* items = *(void**)(cp + 0x10);
        int32_t size = *(int32_t*)(cp + 0x18);

        if (!items || size <= 0) {
            Logging::error("Items is null or size is 0");
            return oSetupChoices(instance, dialogChoiceObject, methodInfo);
        }

        bool isMajorChoice = *(bool*)(dc + 0x1B0);
        bool autoComplete  = *(bool*)(dc + 0x1B1);

        uintptr_t arrayBase = (uintptr_t)items + 0x20;
        auto server = Server::get();
        std::vector<Choice> packetChoices;
        std::vector<std::pair<Choice, float>> choiceVotes;

        VotesPanel::get()->setChoiceMade({
            oChoiceMade,
            instance,
            eInteractMenu::kLeft,
            methodInfo
        });

        for (int i = 0; i < choices->size; i++) {
            ChoiceObject* choice = *(ChoiceObject**)(arrayBase + i * sizeof(void*));
            if (!choice) continue;

            eChoicePositions pos = oGetPreferredChoice((void*)choice, nullptr);

            Il2CppString* text = *(Il2CppString**)((uintptr_t)choice + 0x68);
            if (!text) continue;

            Choice packetChoice = {
                .title = ReadIl2CppString(text),
                .choiceID = i,
                .choicePos = (int)pos
            };

            packetChoices.push_back(packetChoice);
            choiceVotes.push_back({
                packetChoice,
                0.f
            });
        }

        if (server->isOnline()) {
            server->setChoices(choiceVotes);
            server->setTotalVotes(0);
            VotesPanel::get()->showPanel(true);

            VotesPanel::get()->startTimer(GameManager::get()->choiceTime);

            auto packet = SendChoicesPacket::create(packetChoices, isMajorChoice);
            server->broadcast(packet);
        }

        return oSetupChoices(instance, dialogChoiceObject, methodInfo);
    }

    void createBTSChoicesHooks(LPVOID *pSetupChoices, ChoiceMade_t ogChoiceMade, GetDialogChoice_t ogDialogChoice, GetPreferredChoice_t ogGetPreferredChoice) {
        oChoiceMade = ogChoiceMade;
        oGetDialogChoice = ogDialogChoice;
        oGetPreferredChoice = ogGetPreferredChoice;

        MH_CreateHook(pSetupChoices, hkSetupChoices, reinterpret_cast<LPVOID*>(&oSetupChoices));
        MH_EnableHook(pSetupChoices);
    }
}

namespace LifeIsStrangeRemastered {
    fnProcessEvent oProcessEvent = nullptr;
    void __stdcall hkProcessEvent(SDK::UObject* thisObj, SDK::UFunction* function, void* params) {
        __try {
            // BP_LiSDialogWidget_View.BP_LiSDialogWidget_View_C.Construct
            if (thisObj && function) {
                std::string funcName = function->GetFullName();
                if (funcName.find("Tick") == std::string::npos &&
                    funcName.find("Timer") == std::string::npos &&
                    funcName.find("BlueprintUpdateAnimation") == std::string::npos &&
                    funcName.find("UMG") == std::string::npos
                ) {
                    Logging::info("ProcessEvent called: {}", funcName);
                }

                if (funcName.find("BP_LiSDialogWidget_View.BP_LiSDialogWidget_View_C.UpdateTextImplementable") != std::string::npos) {
                    Logging::info("BP_LiSDialogWidget_View.BP_LiSDialogWidget_View_C.UpdateTextImplementable called.");
                    SDK::ULiSDialogWidget* object = static_cast<SDK::ULiSDialogWidget*>(thisObj);
                    for (auto choice : object->DialogChoices) {
                        auto localizedString = object->GetLocalizedString(choice);
                        Logging::info("Choice Localized: {}", localizedString.ToString());
                    }
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            DWORD code = GetExceptionCode();
            printf("Exception in hook logging: 0x%08lX\n", code);
            printf("thisObj=%p  function=%p  parms=%p\n", (void*)thisObj, (void*)function, params);
        }

        oProcessEvent(thisObj, function, params);
    }

    fnProcessEvent GetProcessEventFromVTable() {
        SDK::UObject* obj = SDK::UObject::GObjects->GetByIndex(0);
        if (!obj) return nullptr;

        void** vtable = *reinterpret_cast<void***>(obj);
        return reinterpret_cast<fnProcessEvent>(vtable[68]);
    }

    void createLISRemasteredChoicesHooks() {
        fnProcessEvent target = GetProcessEventFromVTable();
        if (!target) {
            Logging::error("Failed to get ProcessEvent address from VTable");
            return;
        }

        if (MH_CreateHook(
            reinterpret_cast<LPVOID>(target),
            reinterpret_cast<LPVOID>(&hkProcessEvent),
            reinterpret_cast<LPVOID*>(&oProcessEvent)
        ) != MH_OK) {
            Logging::error("ProcessEvent hook creation failed.");
            return;
        }

        if (MH_EnableHook(reinterpret_cast<LPVOID>(target)) != MH_OK) {
            Logging::error("ProcessEvent hook enable failed.");
            return;
        }

        Logging::info("ProcessEvent hooked");
    }
}