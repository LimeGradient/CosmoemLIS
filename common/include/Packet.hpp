#pragma once

#include <nlohmann/json.hpp>

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