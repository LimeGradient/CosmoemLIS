#include "UI/Window.hpp"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include "DiscordRPC.hpp"
#include "Log.hpp"
#include "UI/UIHelper.hpp"
#include "UI/fonts/Arial.hpp"
#include "UI/panels/ConnectPanel.hpp"
#include "UI/panels/ChoicePanel.hpp"

void Window::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if (!SDL_CreateWindowAndRenderer("LIS Multiplayer - Client", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT, &this->window, &this->renderer)) {
        Logging::error("Couldn't create window or renderer: {}", SDL_GetError());
        return;
    }

    SDL_SetRenderLogicalPresentation(this->renderer, 1280, 720, SDL_LOGICAL_PRESENTATION_DISABLED);

    SDL_Surface* icon = SDL_LoadBMP("resources/favicon.bmp");
    SDL_SetWindowIcon(window, icon);
    SDL_DestroySurface(icon);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontDefault();
    auto arialFont = io.Fonts->AddFontFromMemoryCompressedTTF(arials_compressed_data, sizeof(arials_compressed_size), 20);
    
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplSDL3_InitForSDLRenderer(this->window, this->renderer);
    ImGui_ImplSDLRenderer3_Init(this->renderer);

    SDL_Texture* background;
    int backgroundWidth, backgroundHeight;
    bool ret = UIHelper::LoadTextureFromFile("resources/background_1.jpg", renderer, &background, &backgroundWidth, &backgroundHeight);
    IM_ASSERT(ret);

    auto discordManager = DiscordManager::get();
    auto presence = discord::Presence();
    presence.setState("LIS Multiplayer - Client");
    presence.setDetails("Waiting to connect...");
    presence.setLargeImageKey("favicon");
    discordManager->setPresence(presence);

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

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        ImVec2 windowSize(w, h);
        UIHelper::createPanel("MainPanel", ImVec2(950, 500), UIHelper::getCenter(windowSize, ImVec2(950, 500)), [windowSize, arialFont]() {
            ImGui::SetCursorPos(UIHelper::getCenter(ImVec2(950, 500), ImVec2(900, 450)));
            ImGui::BeginChild("Title", ImVec2(900, 450), true, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs );
            
            ImGui::PushFont(arialFont);
            std::string title = "LIS Multiplayer - Client";
            float x = (ImGui::GetWindowSize().x / 2) - (ImGui::CalcTextSize(title.c_str()).x / 2);
            UIHelper::renderText(title, arialFont, ImVec2(x, 10));
            ImGui::PopFont();

            ImGui::Separator();

            auto connectPanelPos = UIHelper::getCenter(windowSize, ImVec2(CONNECT_PANEL_WIDTH, CONNECT_PANEL_HEIGHT));
            ConnectPanel::get()->init(connectPanelPos.x, connectPanelPos.y);

            ImGui::EndChild();
        }, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
        
        ChoicePanel::get()->render();

        ImGui::Render();
        SDL_SetRenderDrawColorFloat(this->renderer, 0, 0, 0, (!isTransparent) ? SDL_ALPHA_OPAQUE_FLOAT : SDL_ALPHA_TRANSPARENT_FLOAT);
        SDL_RenderClear(this->renderer);
        SDL_RenderTexture(renderer, background, NULL, NULL);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(this->renderer);
    }

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}   