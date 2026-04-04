#include "UI/Window.hpp"

#include "network/Client.hpp"
#include "packets/Client.hpp"

#include <stdio.h>

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return;
    }

    if (!SDL_CreateWindowAndRenderer("LIS Multiplayer - Client", 1280, 720, SDL_WINDOW_RESIZABLE, &this->window, &this->renderer)) {
        printf("Couldn't create window or renderer: %s", SDL_GetError());
        return;
    }

    SDL_SetRenderLogicalPresentation(this->renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    ClientManager* cm = ClientManager::get();
    cm->init();
    cm->send(UserJoinPacket::create("limegradient", "uhh-idk-fix-this"));

    SDL_Event event;
    this->running = true;
    while (this->running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                this->running = false;
            }
        }
        
        SDL_SetRenderDrawColorFloat(this->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT);
        SDL_RenderClear(this->renderer);
        SDL_RenderPresent(this->renderer);
    }

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}   