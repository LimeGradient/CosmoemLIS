#pragma once

#include <nlohmann/json.hpp>

namespace Cosmoem {
    template <typename Derived, size_t PacketID>
    class Packet {
    public:
        nlohmann::json encode() const {
            return nlohmann::json::object({
                {"packetID", PacketID},
                {"data", static_cast<const Derived*>(this)->encodeData()}
            });
        }

    protected:
        virtual nlohmann::json encodeData() const {
            return nlohmann::json::object({
                {"error", std::format("Packet {} has not been implemented", typeid(static_cast<const Derived*>(this)).name())}
            });
        }
    };
}