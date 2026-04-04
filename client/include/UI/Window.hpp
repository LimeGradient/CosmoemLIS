#pragma once

#include <SDL3/SDL.h>

class Window {
public:
    static Window* get() {
        static Window instance;
        return &instance;
    }

    void init();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
};