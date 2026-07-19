#include "server/WinCapture.hpp"

#include <chrono>
#include <thread>

#include "Logging.hpp"
#include "server/WinCaptureServer.hpp"
#include "types/Image4C.hpp"
#include "utils/Win32Utils.hpp"

namespace Cosmoem {
    WinCapture::Frame WinCapture::captureWindow(HWND hwnd) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        HDC hwndDC = GetWindowDC(hwnd);
        HDC saveDC = CreateCompatibleDC(hwndDC);

        auto hBitmap = CreateCompatibleBitmap(hwndDC, width, height);
        SelectObject(saveDC, hBitmap);

        auto res = PrintWindow(hwnd, saveDC, 2);

        BITMAP bitmapInfo;
        GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);

        LONG bufferSize = bitmapInfo.bmWidthBytes * bitmapInfo.bmHeight;
        BYTE* pixelBuffer = new BYTE[bufferSize];
        GetBitmapBits(hBitmap, bufferSize, pixelBuffer);

        Image4C img(pixelBuffer, bitmapInfo.bmHeight, bitmapInfo.bmWidth);

        DeleteObject(hBitmap);
        DeleteDC(saveDC);
        ReleaseDC(hwnd, hwndDC);

        return {
            img.data(),
            bitmapInfo.bmWidth,
            bitmapInfo.bmHeight
        };
    }

    void WinCapture::startCapture() {
        HWND hwnd = this->gameHWND;

        const double targetFps = 30.0;
        const auto frameTime = std::chrono::duration<double>(1.0 / targetFps);

        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            if (!IsWindow(hwnd)) {
                Logging::error("Target window no longer exists");
                break;
            }

            Frame frame;
            try {
                frame = this->captureWindow(hwnd);
            } catch (const std::runtime_error& e) {
                Logging::error("Capture error: {}", e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            uint32_t dataLen = static_cast<uint32_t>(frame.data.size());
            uint32_t w = static_cast<uint32_t>(frame.width);
            uint32_t h = static_cast<uint32_t>(frame.height);

            std::vector<uint8_t> packet;
            packet.reserve(12 + frame.data.size());
            packet.resize(12);
            std::memcpy(packet.data() + 0, &dataLen, 4);
            std::memcpy(packet.data() + 4, &w, 4);
            std::memcpy(packet.data() + 8, &h, 4);
            packet.insert(packet.end(), frame.data.begin(), frame.data.end());

            if (WinCaptureServer::get()->isOnline()) {
                WinCaptureServer::get()->sendData(packet);
            }
        }

        return;
    }
}