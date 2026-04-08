#pragma once

#include <string>
#include <vector>

#include "Packet.hpp"

#include "types/Choice.hpp"

struct User {
    std::string name;
    std::string userID;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, name, userID);
};

class UserJoinedPacket : public Packet<UserJoinedPacket, 2001> {
public:
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

class UserLeftPacket : public Packet<UserLeftPacket, 2002> {
public:
    UserLeftPacket(std::vector<User> users) : users(users) {}

    static UserLeftPacket create(std::vector<User> users) {
        return UserLeftPacket(users);
    }

    friend class Packet;

private:
    nlohmann::json encodeData() const {
        return nlohmann::json::object({
            {"users", users}
        });
    }

    std::vector<User> users;
};

class SendChoicesPacket : public Packet<SendChoicesPacket, 3001> {
public:
    SendChoicesPacket(std::vector<Choice> choices, bool isMajorChoice) : choices(choices), isMajorChoice(isMajorChoice) {}

    static SendChoicesPacket create(std::vector<Choice> choices, bool isMajorChoice) {
        return SendChoicesPacket(choices, isMajorChoice);
    }

    friend class Packet;

private:
    nlohmann::json encodeData() const {
        return nlohmann::json::object({
            {"choices", choices},
            {"isMajorChoice", isMajorChoice}
        });
    }

    std::vector<Choice> choices;
    bool isMajorChoice;
};