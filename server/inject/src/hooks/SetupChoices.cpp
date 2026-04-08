#include "hooks/SetupChoices.hpp"

#include <atomic>
#include <MinHook.h>

#include "packets/Server.hpp"
#include "server/Server.hpp"

#include "panel/VotesPanel.hpp"

bool(__stdcall* oSetupChoices)(void*, DialogChoiceGraphObject*, void*);
bool hkSetupChoices(void* instance, DialogChoiceGraphObject* dialogChoiceObject, void* methodInfo);

ChoiceMade_t oChoiceMade = nullptr;
GetDialogChoice_t oGetDialogChoice = nullptr;
GetPreferredChoice_t oGetPreferredChoice = nullptr;

std::atomic<bool> timerSpawned = false;

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

    bool isMajorChoice = *(bool*)(dc + 0x1B0);
    bool autoComplete  = *(bool*)(dc + 0x1B1);
    printf("isMajorChoice: %d\n", isMajorChoice);
    printf("autoComplete:  %d\n", autoComplete);

    uintptr_t arrayBase = (uintptr_t)items + 0x20;
    auto server = Server::get();
    std::vector<Choice> packetChoices;
    std::vector<std::pair<Choice, float>> choiceVotes;

    for (int i = 0; i < choices->size; i++) {
        ChoiceObject* choice = *(ChoiceObject**)(arrayBase + i * sizeof(void*));
        if (!choice) continue;

        eChoicePositions pos = oGetPreferredChoice((void*)choice, nullptr);

        Il2CppString* text = *(Il2CppString**)((uintptr_t)choice + 0x68);
        if (!text) continue;

        printf("choice[%d] text: %s - position: %d\n", i, ReadIl2CppString(text).c_str(), (int)pos);

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
        VotesPanel::get()->showPanel(true);

        VotesPanel::get()->startTimer(10);

        auto packet = SendChoicesPacket::create(packetChoices, isMajorChoice);
        server->broadcast(packet);
    }

    // choice auto positioning guide:
    // left -> right -> up -> down
    // oChoiceMade(instance, eInteractMenu::kLeft, methodInfo);
    return oSetupChoices(instance, dialogChoiceObject, methodInfo);
}

void createChoicesHooks(LPVOID *pSetupChoices, ChoiceMade_t ogChoiceMade, GetDialogChoice_t ogDialogChoice, GetPreferredChoice_t ogGetPreferredChoice) {
    oChoiceMade = ogChoiceMade;
    oGetDialogChoice = ogDialogChoice;
    oGetPreferredChoice = ogGetPreferredChoice;

    MH_CreateHook(pSetupChoices, hkSetupChoices, reinterpret_cast<LPVOID*>(&oSetupChoices));
    MH_EnableHook(pSetupChoices);
}