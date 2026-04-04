#pragma once

#include <string>
#include <vector>

#include "Packet.hpp"

class UserJoinedPacket : public Packet<UserJoinedPacket, 2001> {
public:
    struct User {
        std::string name;
        std::string userID;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, name, userID);
    };

    UserJoinedPacket(std::vector<User> users) : users(users) {}

    static UserJoinedPacket create(std::vector<User> users) {
        return UserJoinedPacket(users);
    }

    friend class Packet;

protected:
    nlohmann::json encodeData() const {
        return nlohmann::json::object({
            {"users", users}
        });
    }

   std::vector<User> users;
};