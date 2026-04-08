#pragma once

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <vector>

#include "packets/Server.hpp"

class ClientManager {
public:
    static ClientManager* get() {
        static ClientManager instance;
        return &instance;
    }

    void init(std::string host, int port, std::string name);
    bool connected() {
        if (this->webSocket.getReadyState() == ix::ReadyState::Open) {
            return true;
        } else {
            return false;
        }
    }
    
    template <typename Packet>
    void send(Packet const& packet) {
        std::string val = packet.encode().dump();
        auto res = this->webSocket.sendText(val);
    }

    std::string getUserID() {
        return this->userID;
    }

    std::vector<User> getClients() {
        return this->clients;
    }

    void setClients(std::vector<User> clients) {
        this->clients = clients;
    }

private:
    void handlePackets(std::string packetData);

    ~ClientManager();

    ix::WebSocket webSocket;
    std::string userID;
    std::vector<User> clients;
};