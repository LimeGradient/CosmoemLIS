#include "network/Client.hpp"

#include <format>
#include <stdio.h>

#include "Log.hpp"
#include "Packet.hpp"
#include "packets/Client.hpp"
#include "StringExtras.hpp"
#include "UI/panels/ChoicePanel.hpp"

void ClientManager::init(std::string host, int port, std::string name) {
    ix::initNetSystem();

    this->webSocket.setUrl(std::format("ws://{}:{}/", host, port));
    this->webSocket.setPingInterval(45);
    this->webSocket.setOnMessageCallback([this, name](const ix::WebSocketMessagePtr& msg) {
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
                Logging::info("Connection established with server");
                this->send(UserJoinPacket::create(name, this->getUserID()));
                break;
            case ix::WebSocketMessageType::Close:
                Logging::info("Connection closed with server - code: {} | reason: {} | remote: {}",
                    msg->closeInfo.code,
                    msg->closeInfo.reason.c_str(),
                    msg->closeInfo.remote ? "client closed" : "server closed"
                );
                ChoicePanel::get()->showPanel(false);
                break;
            case ix::WebSocketMessageType::Error:
                Logging::error("Error - code: {} | reason: {}",
                    msg->errorInfo.http_status,
                    msg->errorInfo.reason.c_str()
                );
            case ix::WebSocketMessageType::Ping:
            case ix::WebSocketMessageType::Pong:
            case ix::WebSocketMessageType::Fragment:
            break;
        }
    });

    Logging::info("Starting websocket client");
    this->webSocket.start();
    this->userID = StringExtras::generateRandomString(16);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ClientManager::handlePackets(std::string packetData) {
    nlohmann::json packetRaw = nlohmann::json::parse(packetData);
    
    if (!packetRaw.contains("packetID")) {
        Logging::error("No packet data found in packet.");
        Logging::info("Recieved data: {}", packetData);
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