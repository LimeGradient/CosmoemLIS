#pragma once

#include <atomic>
#include <string>

#include "types/Choice.hpp"

#include "hooks/SetupChoices.hpp"

struct ChoiceMadeFunction {
    BeforeTheStormRemastered::ChoiceMade_t oChoiceMade;
    void* instance;
    eInteractMenu button;
    void* methodInfo;
};

class VotesPanel {
public:
    static VotesPanel* get() {
        static VotesPanel instance;
        return &instance;
    }

    void render();
    void startTimer(int seconds);

    void setChoices(std::vector<std::pair<Choice, float>> choices) {
        this->choices = choices;
    }

    void showPanel(bool show) {
        this->_showPanel = show;
    }
    
    void setChoiceMade(ChoiceMadeFunction choiceMade) {
        this->oChoiceMade = choiceMade;
    }

    void setChoiceMade(std::function<void(int)> choiceChoosed) {
        this->oChoiceChoosed = choiceChoosed;
    }

    void makeChoice(eInteractMenu button);
    void makeChoice(int choiceIndex);

private:
    bool _showPanel = false;
    std::vector<std::pair<Choice, float>> choices = {};
    std::atomic<bool> timerComplete = false;
    std::atomic<bool> choiceMade = false;
    std::atomic<bool> timerThreadSpawned = false;
    std::string timerStr = "0";

    ChoiceMadeFunction oChoiceMade;
    std::function<void(int)> oChoiceChoosed;
};