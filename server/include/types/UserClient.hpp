#pragma once

#include <string>
#include <ixwebsocket/IXWebSocket.h>

#include "packets/Server.hpp"

class UserClient {
public:
    UserClient(std::string name, std::string userID, ix::WebSocket& socket) : name(name), userID(userID) {}

    std::string name;
    std::string userID;
};