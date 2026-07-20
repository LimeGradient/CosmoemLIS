#pragma once

#include <vector>

#include <SDL3/SDL.h>

namespace Cosmoem {
    class GameCaptureWindow {
    public:
        struct FrameData {
            uint32_t& textureWidth;
            uint32_t& textureHeight;
            std::vector<uint8_t>& data;
            uint32_t width;
            uint32_t height;

            FrameData(
                uint32_t& texWidth, uint32_t& texHeight, std::vector<uint8_t>& pixels, uint32_t width, uint32_t height
            ) : textureWidth(texWidth), textureHeight(texHeight), data(pixels), width(width), height(height) {}
        };

        void init();
        void renderFrame(FrameData frameData);
    
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
    };
}