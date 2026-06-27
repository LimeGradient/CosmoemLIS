#include "hooks/SetupChoices.hpp"

#include <atomic>
#include <MinHook.h>

#include "Log.hpp"
#include "packets/Server.hpp"
#include "panel/VotesPanel.hpp"
#include "server/Server.hpp"
#include "server/GameManager.hpp"

namespace LifeIsStrangeRemastered {
    std::atomic<bool> timerSpawned = false;

    template <typename T>
    T* FindLiveInstance() {
        SDK::UClass* TargetClass = T::StaticClass();
        SDK::UObject* DefaultObj = reinterpret_cast<SDK::UObject*>(T::GetDefaultObj());

        const SDK::int32 Count = SDK::UObject::GObjects->Num();

        for (SDK::int32 i = 0; i < Count; i++)
        {
            SDK::UObject* Obj = SDK::BasicFilesImplUtils::GetObjectByIndex(i);
            if (!Obj)
                continue;

            if (Obj == DefaultObj)
                continue; // skip the CDO/template instance

            if (Obj->Class != TargetClass)
                continue; // exact-class match only

            return static_cast<T*>(Obj);
        }

        return nullptr;
    }

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
                    static auto lastTime = std::chrono::steady_clock::now();
                    const double waitTime = 1.0;

                    auto currentTime = std::chrono::steady_clock::now();
                    auto elapsed = currentTime - lastTime;

                    VotesPanel::get()->setChoiceMade(
                        [](int i) {
                            SDK::ALiSDialogActor* dialogActor = FindLiveInstance<SDK::ALiSDialogActor>();
                            dialogActor->OnDialogChoiceNodeChoiceChoosed(i);
                        }
                    );

                    if (elapsed.count() >= waitTime) {
                        lastTime = currentTime;

                        SDK::ULiSDialogWidget* object = static_cast<SDK::ULiSDialogWidget*>(thisObj);

                        std::vector<Choice> packetChoices;
                        std::vector<std::pair<Choice, float>> choiceVotes;

                        auto server = Server::get();

                        for (SDK::int32 i = 0; i < object->DialogChoices.Num(); i++) {
                            auto localizedString = object->GetLocalizedString(object->DialogChoices[i]);
                            
                            if (localizedString.ToString().empty())
                                continue;

                            Choice packetChoice = {
                                .title = localizedString.ToString(),
                                .choiceID = i,
                                .choicePos = i
                            };
                            packetChoices.push_back(packetChoice);
                            choiceVotes.push_back({
                                packetChoice,
                                0.f
                            });
                            
                            Logging::info("Index: {} | Choice Localized: {}", i, localizedString.ToString());
                        }

                        if (server->isOnline()) {
                            server->setChoices(choiceVotes);
                            server->setTotalVotes(0);
                            VotesPanel::get()->showPanel(true);

                            VotesPanel::get()->startTimer(GameManager::get()->choiceTime);

                            auto packet = SendChoicesPacket::create(packetChoices, object->IsMajorChoice());
                            server->broadcast(packet);
                        }
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