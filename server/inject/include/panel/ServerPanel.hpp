#pragma once

#include <string>

class ServerPanel {
public:
    static ServerPanel* get() {
        static ServerPanel instance;
        return &instance;
    }

    void renderHostPanel();

private:
    void startServer();

    int port;
    std::string name;
};