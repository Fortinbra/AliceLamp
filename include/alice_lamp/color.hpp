#pragma once

#include <cstdint>

namespace alice_lamp {

struct Color {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

struct ColorSample {
    Color color;
    float intensity;
};

}  // namespace alice_lamp