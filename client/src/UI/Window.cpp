#include "UI/Window.hpp"

#include <stdio.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include "UI/panels/ConnectPanel.hpp"
#include "UI/panels/ChoicePanel.hpp"

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if (!SDL_CreateWindowAndRenderer("LIS Multiplayer - Client", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT, &this->window, &this->renderer)) {
        printf("Couldn't create window or renderer: %s", SDL_GetError());
        return;
    }

    SDL_SetRenderLogicalPresentation(this->renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_DISABLED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplSDL3_InitForSDLRenderer(this->window, this->renderer);
    ImGui_ImplSDLRenderer3_Init(this->renderer);

    SDL_Event event;
    this->running = true;
    while (this->running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                this->running = false;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        
        ConnectPanel::get()->render();
        ChoicePanel::get()->render();

        ImGui::Render();
        SDL_SetRenderDrawColorFloat(this->renderer, 0, 0, 0, (!isTransparent) ? SDL_ALPHA_OPAQUE_FLOAT : SDL_ALPHA_TRANSPARENT_FLOAT);
        SDL_RenderClear(this->renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(this->renderer);
    }

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}   