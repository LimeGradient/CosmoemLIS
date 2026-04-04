#pragma once

#include <any>
#include <memory>
#include <vector>

#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

#include "types/UserClient.hpp"

class Server {
public:
    static Server* get() {
        static Server instance;
        return &instance;
    }

    void init(int port);

private:
    template <typename Packet>
    void send(Packet const& packet, ix::WebSocket socket) {
        std::string val = packet.encode().dump();
        auto res = socket.sendText(val);
    }

    template <typename Packet>
    void broadcast(Packet const& packet) {
        for (auto socket : this->webSocket->getClients()) {
            std::string val = packet.encode().dump();
            auto res = socket->sendText(val);
        }
    }

    void handlePackets(std::string packetData, ix::WebSocket& socket);
    void handleUserJoinPacket(nlohmann::json rawData, ix::WebSocket& socket);

    std::unique_ptr<ix::WebSocketServer> webSocket;
    std::vector<UserClient> clients;
};