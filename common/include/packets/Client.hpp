#pragma once

#include "Packet.hpp"
#include "types/UserClient.hpp"

class UserJoinPacket : public Packet<UserJoinPacket, 1001> {
public:
    UserJoinPacket(std::string name, std::string userID) : name(name), userID(userID) {}

    static UserJoinPacket create(std::string name, std::string userID) {
        return UserJoinPacket(name, userID);
    }

    friend class Packet;

protected:
    nlohmann::json encodeData() const {
        return nlohmann::json::object({
            {"name", name},
            {"userID", userID}
        });
    }

    std::string name;
    std::string userID;
};