#include "network/Client.hpp"

#include <stdio.h>

#include "packets/Client.hpp"

void ClientManager::init() {
    ix::initNetSystem();

    this->webSocket.setUrl("ws://127.0.0.1:2876/");
    this->webSocket.setPingInterval(45);
    this->webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message:
                printf("Message: %s\n", msg->str.c_str());
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}