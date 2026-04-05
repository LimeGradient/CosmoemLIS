#include "StringExtras.hpp"

#include <random>
#include <algorithm>

std::string StringExtras::generateRandomString(size_t length) {
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, charset.size() - 1);
    std::string result;
    std::generate_n(std::back_inserter(result), length, [&]() {
        return charset[dist(gen)];
    });
    
    return result;
}