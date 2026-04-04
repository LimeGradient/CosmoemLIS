#include "server/Server.hpp"

#include <stdio.h>

void Server::init(int port) {
    this->webSocket = std::make_unique<ix::WebSocketServer>(port, "0.0.0.0");

    this->webSocket->setOnClientMessageCallback([](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {
        switch (msg->type) {
            case ix::WebSocketMessageType::Message:
                printf("msg: %s\n", msg->str.c_str());
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
    printf("Server started on port %d\n", port);
}