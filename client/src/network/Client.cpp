#include "network/Client.hpp"

#include <stdio.h>
#include <format>

#include "Packet.hpp"
#include "packets/Client.hpp"
#include "StringExtras.hpp"
#include "UI/panels/ChoicePanel.hpp"

void ClientManager::init(std::string host, int port) {
    ix::initNetSystem();

    this->webSocket.setUrl(std::format("ws://{}:{}/", host, port));
    this->webSocket.setPingInterval(45);
    this->webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message:
                if (!msg->binary || !msg->str.empty()) {
                    auto type = (MessageType)(uint8_t)msg->str[0];
                    std::string payload = msg->str.substr(1);
                    switch (type) {
                        case MessageType::Packet:
                            this->handlePackets(payload);
                            break;
                        case MessageType::Frame:
                            break;
                    }
                }
                break;
            case ix::WebSocketMessageType::Open:
                printf("Connection established with server\n");
                break;
            case ix::WebSocketMessageType::Close:
                printf(
                    "Connection closed with server - code: %hu | reason: %s\n | remote: %s", 
                    msg->closeInfo.code, 
                    msg->closeInfo.reason.c_str(),
                    msg->closeInfo.remote ? "client closed" : "server closed"
                );
                ChoicePanel::get()->showPanel(false);
                this->webSocket.stop();
                break;
            case ix::WebSocketMessageType::Error:
            case ix::WebSocketMessageType::Ping:
            case ix::WebSocketMessageType::Pong:
            case ix::WebSocketMessageType::Fragment:
            break;
        }
    });

    printf("Starting websocket client...\n");
    this->webSocket.start();
    this->userID = StringExtras::generateRandomString(16);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ClientManager::handlePackets(std::string packetData) {
    nlohmann::json packetRaw = nlohmann::json::parse(packetData);
    
    if (!packetRaw.contains("packetID")) {
        printf("No packet id found in message: %s\n", packetData.c_str());
        return;
    }

    int packetID = packetRaw["packetID"];
    switch (packetID) {
        case 2001: {
            std::vector<User> users = packetRaw["data"]["users"];
            this->setClients(users);
            break;
        }
        case 2002: {
            std::vector<User> users = packetRaw["data"]["users"];
            this->setClients(users);
            break;
        }
        case 3001: {
            std::vector<Choice> choices = packetRaw["data"]["choices"];
            ChoicePanel::get()->setChoices(choices);
            ChoicePanel::get()->showPanel(true);
            break;
        }
    }
}

ClientManager::~ClientManager() {
    ChoicePanel::get()->showPanel(false);
    this->send(UserLeavePacket::create(this->userID));
    this->webSocket.stop();
    ix::uninitNetSystem();
}