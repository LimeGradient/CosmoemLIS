#pragma once

#include <string>

class ConnectPanel {
public:
    static ConnectPanel* get() {
        static ConnectPanel instance;
        return &instance;
    }

    void render();

private:
    std::string host;
    int port;
    std::string name;
};