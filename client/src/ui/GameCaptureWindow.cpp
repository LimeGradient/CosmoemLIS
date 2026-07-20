#include "ui/GameCaptureWindow.hpp"

#include <ixwebsocket/IXWebSocket.h>

namespace Cosmoem {
    void GameCaptureWindow::init() {
        this->window = SDL_CreateWindow("CosmoemLIS", 1280, 720, SDL_WINDOW_RESIZABLE);
        this->renderer = SDL_CreateRenderer(window, nullptr);

        SDL_Texture* texture = nullptr;
        uint32_t texWidth = 0, texHeight = 0;

        ix::WebSocket ws;
        ws.setUrl("ws://127.0.0.1:2877");

        ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
            if (msg->type != ix::WebSocketMessageType::Message || !msg->binary) {
                return;
            }

            const char* raw = msg->str.data();
            size_t len = msg->str.size();
            if (len < 12) return;

            uint32_t dataLen, width, height;
            std::memcpy(&dataLen, raw + 0, 4);
            std::memcpy(&width,   raw + 4, 4);
            std::memcpy(&height,  raw + 8, 4);
            if (len < 12 + dataLen) return;

            std::vector<uint8_t> pixels(
                reinterpret_cast<const uint8_t*>(raw) + 12,
                reinterpret_cast<const uint8_t*>(raw) + 12 + dataLen
            );

            this->renderFrame(FrameData(texWidth, texHeight, pixels, width, height));
        });

        ws.start();
        
        bool running = true;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }
            SDL_Delay(1);
        }

        if (texture) SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        ws.stop();
        return;
    }

    void GameCaptureWindow::renderFrame(GameCaptureWindow::FrameData frameData) {
        if (!this->texture || frameData.width != frameData.textureWidth || frameData.height != frameData.textureHeight) {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
            texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_BGRA32,
                SDL_TEXTUREACCESS_STREAMING,
                static_cast<int>(frameData.width),
                static_cast<int>(frameData.height)
            );
            frameData.textureWidth = frameData.width;
            frameData.textureHeight = frameData.height;
        }
        int pitch = static_cast<int>(frameData.width) * 4;
        SDL_UpdateTexture(texture, nullptr, frameData.data.data(), pitch);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}