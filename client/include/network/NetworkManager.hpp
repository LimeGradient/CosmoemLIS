#pragma once

#include <string>
#include <vector>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

#include "utils/Singleton.hpp"

namespace Cosmoem {
    class NetworkManager : public Utils::Singleton<NetworkManager> {
        void init(std::string host, int port, std::string name);
    };
}