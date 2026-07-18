#pragma once

#include <memory>
#include <vector>

#include <ixwebsocket/IXWebSocketServer.h>

#include "utils/Singleton.hpp"

namespace Cosmoem {
    class WinCaptureServer : public Utils::Singleton<WinCaptureServer> {
    public:
        void start(int port);
        void stop();
        bool isOnline();

        void sendData(std::vector<uint8_t> frameData);
    
    private:
        std::unique_ptr<ix::WebSocketServer> webSocket;
        bool online = false;
    };
}