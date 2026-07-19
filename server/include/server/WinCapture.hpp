#pragma once

#include "utils/Singleton.hpp"
#include "utils/GameUtils.hpp"

#include <vector>

#include <Windows.h>

namespace Cosmoem {
    class WinCapture : public Utils::Singleton<WinCapture> {
    public:
        struct Frame {
            std::vector<uint8_t> data;
            int width;
            int height;
        };

        Frame captureWindow(HWND hwnd);
        void startCapture();
    
        void setGameHWND(HWND hwnd) {
            this->gameHWND = hwnd;
        }

    private:
        HWND gameHWND;
    };
}