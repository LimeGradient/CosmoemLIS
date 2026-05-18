#pragma once

#include <string>

#define CONNECT_PANEL_WIDTH 750
#define CONNECT_PANEL_HEIGHT 375

class ConnectPanel {
public:
    static ConnectPanel* get() {
        static ConnectPanel instance;
        return &instance;
    }

    void init(float x, float y);
    void render();

private:
    std::string host;
    int port;
    std::string name;
};