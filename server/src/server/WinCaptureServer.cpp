#include "server/WinCaptureServer.hpp"

#include "Logging.hpp"

namespace Cosmoem {
    void WinCaptureServer::start(int port) {
        this->webSocket = std::make_unique<ix::WebSocketServer>(port, "0.0.0.0");

        this->webSocket->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket & webSocket, const ix::WebSocketMessagePtr & msg) {
            switch (msg->type) {
                case ix::WebSocketMessageType::Open:
                    Logging::info("[WinCap] New connection from IP: {}", connectionState->getRemoteIp());
                    break;
                case ix::WebSocketMessageType::Close:
                    Logging::info("[WinCap] Client from IP: {} disconnected", connectionState->getRemoteIp());
                    break;
                case ix::WebSocketMessageType::Message:
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
        Logging::info("[WinCap] Server started on port {}", port);
    }

    void WinCaptureServer::stop() {
        this->webSocket->stop();
        this->online = false;
        Logging::info("[WinCap] Stopping server.");
    }

    bool WinCaptureServer::isOnline() {
        return this->online;
    }

    void WinCaptureServer::sendData(std::vector<uint8_t> frameData) {
        for (auto socket : this->webSocket->getClients()) {
            socket->sendBinary(frameData);
        }
    }
}