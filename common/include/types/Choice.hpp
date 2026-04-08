#pragma once

#include <string>

#include <nlohmann/json.hpp>

struct Choice {
    std::string title;
    int choiceID;
    int choicePos;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Choice, title, choiceID);
};