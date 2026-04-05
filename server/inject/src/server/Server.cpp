#include "server/Server.hpp"

#include "packets/Server.hpp"

#include <stdio.h>

void Server::start(int port) {
    this->webSocket = std::make_unique<ix::WebSocketServer>(port, "0.0.0.0");

    this->webSocket->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message:
                this->handlePackets(msg->str.c_str(), webSocket);
                break;
            case ix::WebSocketMessageType::Open:
                printf("New connection from IP: %s\n", connectionState->getRemoteIp().c_str());
                break;
            case ix::WebSocketMessageType::Close:
                printf("Client from IP: %s disconnected\n", connectionState->getRemoteIp().c_str());
                break;
            case ix::WebSocketMessageType::Error:
            case ix::WebSocketMessageType::Ping:
            case ix::WebSocketMessageType::Pong:
            case ix::WebSocketMessageType::Fragment:
                break;
        }
    });

    auto res = this->webSocket->listen();
    if (!res.first) {
        printf("Server failed to start: %s\n", res.second.c_str());
        return;
    }
    
    this->webSocket->start();
    this->online = true;
    printf("Server started on port %d\n", port);
}

void Server::stop() {
    this->webSocket->stop();
    this->online = false;
    printf("Stopping server...\n");
}

bool Server::isOnline() {
    return this->online;
}

void Server::handlePackets(std::string packetData, ix::WebSocket& socket) {
    printf("Recieved packet: %s\n", packetData.c_str());
    nlohmann::json packetRaw = nlohmann::json::parse(packetData);
    
    if (!packetRaw.contains("packetID")) {
        printf("No packet id found in message: %s\n", packetData.c_str());
        return;
    }

    int packetID = packetRaw["packetID"];
    switch (packetID) {
        case 1001: {
            this->handleUserJoinPacket(packetRaw, socket);
            break;
        }
    }
}

void Server::handleUserJoinPacket(nlohmann::json rawData, ix::WebSocket& socket) {
    printf("Handling user packet\n");
    nlohmann::json data = rawData["data"];

    auto userClient = UserClient(
        data["name"], 
        data["userID"], 
        socket
    );
    this->clients.push_back(userClient);

    std::vector<UserJoinedPacket::User> users;
    for (auto client : this->clients) {
        users.push_back({client.name, client.userID});
    }

    auto packet = UserJoinedPacket(users);
    this->broadcast(packet);
}