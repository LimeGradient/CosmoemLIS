#pragma once

#include <atomic>
#include <string>

#include "types/Choice.hpp"

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

private:
    bool _showPanel = false;
    bool timerComplete = false;
    std::vector<std::pair<Choice, float>> choices = {};
    std::atomic<bool> timerThreadSpawned = false;
    std::string timerStr = "0";
};