#pragma once

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

class ClientManager {
public:
    static ClientManager* get() {
        static ClientManager instance;
        return &instance;
    }

    void init();
    
    template <typename Packet>
    void send(Packet const& packet) {
        std::string val = packet.encode().dump();
        auto res = this->webSocket.sendText(val);
    }

private:
    ix::WebSocket webSocket;
};