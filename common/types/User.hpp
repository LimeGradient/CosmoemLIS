#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace Cosmoem {
    struct User {
        std::string name;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, name);
    };
}