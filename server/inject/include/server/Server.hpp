#pragma once

#include <any>
#include <memory>
#include <vector>

#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

#include "types/UserClient.hpp"
#include "Packet.hpp"

class Server {
public:
    static Server* get() {
        static Server instance;
        return &instance;
    }

    void start(int port);
    void stop();
    bool isOnline();

    std::vector<UserClient> getClients() {
        return this->clients;
    }

    template <typename Packet>
    void send(Packet const& packet, ix::WebSocket socket) {
        std::string val = packet.encode().dump();
        auto res = socket.sendText(val);
    }

    template <typename Packet>
    void broadcast(Packet const& packet) {
        for (auto socket : this->webSocket->getClients()) {
            std::string val = packet.encode().dump();
            auto payload = createPayload(MessageType::Packet, val.data(), val.size());
            auto res = socket->sendBinary(payload);
        }
    }

    void setChoices(std::vector<std::pair<Choice, float>> choices) {
        this->choiceVotes = choices;
    }

    void setTotalVotes(int tv) {
        this->totalVotes = tv;
    }

    std::vector<std::pair<Choice, float>> getChoices() {
        return this->choiceVotes;
    }

    int getTotalVotes() {
        return this->totalVotes;
    }

private:
    void handlePackets(std::string packetData, ix::WebSocket& socket);
    void handleUserJoinPacket(nlohmann::json rawData, ix::WebSocket& socket);
    void handleUserLeavePacket(nlohmann::json rawData);
    void handleSendChoicePacket(nlohmann::json rawData);

    bool online = false;
    std::unique_ptr<ix::WebSocketServer> webSocket;
    std::vector<UserClient> clients;
    std::vector<std::pair<Choice, float>> choiceVotes;
    int totalVotes;
};