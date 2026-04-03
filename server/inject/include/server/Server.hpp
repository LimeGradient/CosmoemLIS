#pragma once

#include <memory>
#include <ixwebsocket/IXWebSocketServer.h>

class Server {
public:
    static Server* get() {
        static Server instance;
        return &instance;
    }

    void init(int port);

private:
    std::unique_ptr<ix::WebSocketServer> webSocket;
};