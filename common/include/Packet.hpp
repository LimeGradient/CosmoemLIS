#pragma once

#include <nlohmann/json.hpp>

enum class MessageType : uint8_t {
    Packet = 0x01,
    Frame = 0x02
};

std::string createPayload(MessageType type, const void* data, size_t size);

template <typename Derived, size_t PacketId>
class Packet {
public:
    nlohmann::json encode() const {
        return nlohmann::json::object({
            {"packetID", PacketId},
            {"data", static_cast<const Derived*>(this)->encodeData()}
        });
    }

    int getPacketID() const {
        return PacketId;
    }
};