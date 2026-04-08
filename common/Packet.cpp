#include "Packet.hpp"
#include "packets/Client.hpp"
#include "packets/Server.hpp"

#include "types/Choice.hpp"

std::string createPayload(MessageType type, const void *data, size_t size) {
    std::string msg(1 + size, '\0');
    msg[0] = (uint8_t)type;
    memcpy(msg.data() + 1, data, size);
    return msg;
}