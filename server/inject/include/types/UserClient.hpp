#pragma once

#include <string>
#include <ixwebsocket/IXWebSocket.h>

class UserClient {
public:
    UserClient(std::string name, std::string userID, ix::WebSocket& socket) : name(name), userID(userID), socket(socket) {}

    std::string name;
    std::string userID;
    ix::WebSocket& socket;
};