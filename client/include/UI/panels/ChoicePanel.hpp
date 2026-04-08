#pragma once

#include <string>

#include "types/Choice.hpp"

class ChoicePanel {
public:
    static ChoicePanel* get() {
        static ChoicePanel instance;
        return &instance;
    }

    void render();

    void setChoices(std::vector<Choice> choices) {
        this->selectedChoice = false;
        this->choices = choices;
    }

    void showPanel(bool show) {
        this->_showPanel = show;
    }

private:
    bool _showPanel = false;
    bool selectedChoice = false;

    std::vector<Choice> choices;
    Choice choice;
};