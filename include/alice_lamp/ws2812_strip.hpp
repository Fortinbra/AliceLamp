#pragma once

#include <cstdint>

#include "alice_lamp/color.hpp"
#include "hardware/pio.h"

namespace alice_lamp {

class Ws2812Strip {
public:
    explicit Ws2812Strip(std::uint32_t data_pin);
    ~Ws2812Strip();

    Ws2812Strip(const Ws2812Strip&) = delete;
    Ws2812Strip& operator=(const Ws2812Strip&) = delete;

    void write(Color color, float intensity);
    void clear(std::uint32_t pixel_count);

private:
    PIO pio_ = pio0;
    std::uint32_t data_pin_;
    std::uint32_t state_machine_;
    std::uint32_t program_offset_;
};

}  // namespace alice_lamp