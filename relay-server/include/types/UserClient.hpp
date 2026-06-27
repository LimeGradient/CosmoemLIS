#pragma once

#include <string>
#include <ixwebsocket/IXWebSocket.h>

class UserClient {
public:
    UserClient(std::string name, std::string userID) : name(name), userID(userID) {}

    void setIsHost(bool isHost) {
        this->isHost = isHost;
    }

    bool getIsHost() {
        return isHost;
    }

    std::string name;
    std::string userID;
    bool isHost;
};