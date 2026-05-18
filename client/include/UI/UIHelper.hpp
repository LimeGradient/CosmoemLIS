#pragma once

#include "thirdparty/stb_image.h"

#include <functional>
#include <string>

#include <imgui.h>
#include <SDL3/SDL.h>

#include <Log.hpp>

class UIHelper {
public:
    static ImVec2 getCenter(ImVec2 windowSize, ImVec2 panelSize) {
        ImVec2 center = ImVec2((windowSize.x - panelSize.x) / 2, (windowSize.y - panelSize.y) / 2);
        return center;
    }

    static void createPanel(std::string id, ImVec2 size, ImVec2 pos, std::function<void()> render, ImGuiWindowFlags flags = 0) {
        ImGui::Begin(id.c_str(), nullptr, flags);
        ImGui::SetWindowSize(size);
        ImGui::SetWindowPos(pos, ImGuiCond_Once);

        render();

        ImGui::End();
    }

    static void renderText(std::string text, ImFont* font, ImVec2 pos) {
        ImGui::SetCursorPos(pos);
        ImGui::PushFont(font);
        ImGui::Text("%s", text.c_str());
        ImGui::PopFont();
    }

    static bool LoadTextureFromMemory(const void* data, size_t data_size, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height) {
        int image_width = 0;
        int image_height = 0;
        int channels = 4;
        unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
        if (image_data == nullptr) {
            Logging::error("Failed to load image: {}", stbi_failure_reason());
            return false;
        }

        SDL_Surface* surface = SDL_CreateSurfaceFrom(image_width, image_height, SDL_PIXELFORMAT_RGBA32, (void*)image_data, channels * image_width);
        if (surface == nullptr) {
            Logging::error("Failed to create SDL surface: {}", SDL_GetError());
            return false;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr)
            Logging::error("Failed to create SDL texture: {}", SDL_GetError());

        *out_texture = texture;
        *out_width = image_width;
        *out_height = image_height;

        SDL_DestroySurface(surface);
        stbi_image_free(image_data);

        return true;
    }

    static bool LoadTextureFromFile(const char* file_name, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height) {
        FILE* f = fopen(file_name, "rb");
        if (f == NULL)
            return false;
        fseek(f, 0, SEEK_END);
        size_t file_size = (size_t)ftell(f);
        if (file_size == -1)
            return false;
        fseek(f, 0, SEEK_SET);
        void* file_data = IM_ALLOC(file_size);
        fread(file_data, 1, file_size, f);
        fclose(f);
        bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
        IM_FREE(file_data);
        return ret;
    }
};