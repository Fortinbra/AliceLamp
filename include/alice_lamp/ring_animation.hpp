#pragma once

#include <cstddef>
#include <cstdint>

#include "alice_lamp/color.hpp"

namespace alice_lamp {

class RingAnimation {
public:
    explicit RingAnimation(std::uint32_t pixel_count);

    void advance(float turns);
    void next_animation();
    [[nodiscard]] ColorSample sample(std::uint32_t pixel_index) const;

private:
    std::uint32_t pixel_count_;
    std::size_t animation_index_ = 0;
    float rotation_ = 0.0F;
};

}  // namespace alice_lamp