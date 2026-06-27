#include "UI/Window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

#include "DiscordRPC.hpp"

int main() {
    auto discordManager = DiscordManager::get();
    discordManager->init();
    const auto presence = discord::RPCManager::get().getPresence()
        .setState("Waiting to connect...")
        .setDetails("LIS Multiplayer - Client")
        .setLargeImageKey("favicon");
    presence.refresh();

    Window* win = Window::get();
    win->init();
}