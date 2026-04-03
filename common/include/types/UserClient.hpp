#pragma once

#include <string>

struct UserClient {
public:
    UserClient(std::string name, std::string userID) : name(name), userID(userID) {}

    std::string name;
    std::string userID;
};