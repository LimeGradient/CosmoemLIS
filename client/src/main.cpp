#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>

#include <ixwebsocket/IXWebSocket.h>
#include "Logging.hpp"
#include "ixwebsocket/IXNetSystem.h"


struct FrameHeader {
    uint32_t dataLen;
    uint32_t width;
    uint32_t height;
};

void renderFrame(SDL_Renderer* renderer, SDL_Texture* texture,
                  uint32_t& texWidth, uint32_t& texHeight,
                  std::vector<uint8_t>& pixels, uint32_t width, uint32_t height)
{
    // Recreate the texture if it doesn't exist yet or the size changed
    if (!texture || width != texWidth || height != texHeight) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_BGRA32,
            SDL_TEXTUREACCESS_STREAMING,
            static_cast<int>(width),
            static_cast<int>(height)
        );
        texWidth = width;
        texHeight = height;
    }

    // Upload the pixel bytes into the texture
    int pitch = static_cast<int>(width) * 4; // BGRA = 4 bytes/pixel
    SDL_UpdateTexture(texture, nullptr, pixels.data(), pitch);

    // Draw it
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

// ---------- Main ----------

int main(int argc, char** argv) {
   // --- SDL setup ---
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Remote Frame Viewer", 1280, 720, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    SDL_Texture* texture = nullptr;
    uint32_t texWidth = 0, texHeight = 0;

    // --- WebSocket setup ---
    ix::initNetSystem();
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

        renderFrame(renderer, texture, texWidth, texHeight, pixels, width, height);
    });

    ws.start();

    // --- Main loop just handles window events; rendering happens in the callback above ---
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
    SDL_Quit();

    ws.stop();
    ix::uninitNetSystem();
    return 0;
}