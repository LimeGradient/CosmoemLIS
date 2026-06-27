#pragma once

#include "types/Game.hpp"

class GameManager {
public:
    static GameManager* get() {
        static GameManager instance;
        return &instance;
    }

    int choiceTime = 30;
    GameType gameType;
};