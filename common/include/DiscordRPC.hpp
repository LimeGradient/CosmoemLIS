#pragma once

#include <discord-rpc.hpp>

#include "Log.hpp"

#define DISCORD_CLIENT_ID "1505943235993014382"

class DiscordManager {
public:
    static DiscordManager* get() {
        static DiscordManager instance;
        return &instance;
    }

    void init() {
        discord::RPCManager::get()
            .setClientID(DISCORD_CLIENT_ID)
            .onReady([this](auto& user) {
                Logging::info("Discord RPC ready: {}", user.username);
            })
            .onErrored([](auto code, auto message) {
                Logging::error("Discord RPC error: {} ({})", message, code);
            })
            .initialize();
    }

    void setPresence(const discord::Presence& presence) {
        discord::RPCManager::get().setPresence(presence);
        discord::RPCManager::get().refresh();
    }

    void clearPresence() {
        discord::RPCManager::get().clearPresence();
    }
};