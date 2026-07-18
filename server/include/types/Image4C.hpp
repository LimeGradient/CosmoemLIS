#pragma once

#include <vector>
#include <cstdint>

namespace Cosmoem {
    class Image4C {
    public:
        Image4C(const uint8_t* data, int height, int width)
            : height_(height), width_(width), data_(data, data + (size_t)height * width * 4) {}

        uint8_t& at(int y, int x, int c) {
            return data_[(size_t)(y * width_ + x) * 4 + c];
        }

        const uint8_t& at(int y, int x, int c) const {
            return data_[(size_t)(y * width_ + x) * 4 + c];
        }

        int height() const { return height_; }
        int width()  const { return width_; }

        std::vector<uint8_t>& data() { return data_; }

    private:
        int height_, width_;
        std::vector<uint8_t> data_;
    };
}