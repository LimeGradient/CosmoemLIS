#include "server/Server.hpp"

#include "Log.hpp"
#include "packets/Server.hpp"

void Server::start(int port) {
    this->webSocket = std::make_unique<ix::WebSocketServer>(port, "0.0.0.0");

    this->webSocket->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message:
                this->handlePackets(msg->str.c_str(), webSocket);
                break;
            case ix::WebSocketMessageType::Open:
                Logging::info("New connection from IP: {}", connectionState->getRemoteIp());
                break;
            case ix::WebSocketMessageType::Close:
                Logging::info("Client from IP: {} disconnected", connectionState->getRemoteIp());
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
        Logging::error("Server failed to start: {}", res.second);
        return;
    }
    
    this->webSocket->start();
    this->online = true;
    Logging::info("Server started on port {}", port);
}

void Server::stop() {
    this->webSocket->stop();
    this->online = false;
    Logging::info("Stopping server.");
}

bool Server::isOnline() {
    return this->online;
}

void Server::handlePackets(std::string packetData, ix::WebSocket& socket) {
    nlohmann::json packetRaw = nlohmann::json::parse(packetData);
    
    if (!packetRaw.contains("packetID")) {
        Logging::error("No packet data found in packet.");
        Logging::info("Recieved data: {}", packetData);
        return;
    }

    int packetID = packetRaw["packetID"];
    switch (packetID) {
        case 1001: {
            this->handleUserJoinPacket(packetRaw, socket);
            break;
        }
        case 1002: {
            this->handleUserLeavePacket(packetRaw);
            break;
        }
        case 3001: {
            this->handleSendChoicePacket(packetRaw);
            break;
        }
    }
}

void Server::handleUserJoinPacket(nlohmann::json rawData, ix::WebSocket& socket) {
    nlohmann::json data = rawData["data"];

    auto userClient = UserClient(
        data["name"], 
        data["userID"], 
        socket
    );
    this->clients.push_back(userClient);

    std::vector<User> users;
    for (auto client : this->clients) {
        users.push_back({client.name, client.userID});
    }

    auto packet = UserJoinedPacket::create(users);
    this->broadcast(packet);
}

void Server::handleUserLeavePacket(nlohmann::json rawData) {
    nlohmann::json data = rawData["data"];

    this->clients.erase(
        std::find_if(this->clients.begin(), this->clients.end(), [data](UserClient const& uc) {
            return uc.userID == data["userID"].get<std::string>();  
        })
    );

    std::vector<User> users;
    for (auto client : this->clients) {
        users.push_back({client.name, client.userID});
    }

    auto packet = UserLeftPacket::create(users);
    this->broadcast(packet);
}

void Server::handleSendChoicePacket(nlohmann::json rawData) {
    nlohmann::json data = rawData["data"];

    auto it = std::find_if(this->choiceVotes.begin(), this->choiceVotes.end(), [data](const std::pair<Choice, float>& c) {
        return c.first.choiceID == data["choice"]["choiceID"];
    });

    if (it != this->choiceVotes.end()) {
        it->second++;
        this->totalVotes++;
    }
}