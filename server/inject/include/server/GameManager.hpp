#pragma once

class GameManager {
public:
    static GameManager* get() {
        static GameManager instance;
        return &instance;
    }

    int choiceTime;
};