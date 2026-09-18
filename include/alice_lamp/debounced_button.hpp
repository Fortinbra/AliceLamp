#pragma once

#include <cstdint>

namespace alice_lamp {

class DebouncedButton {
public:
    DebouncedButton(std::uint32_t pin, bool active_high,
                    std::uint32_t debounce_ms);

    [[nodiscard]] bool pressed(std::uint32_t now_ms);

private:
    [[nodiscard]] bool is_pressed() const;

    std::uint32_t pin_;
    std::uint32_t debounce_ms_;
    std::uint32_t changed_at_ms_;
    bool active_high_;
    bool raw_state_;
    bool stable_state_;
};

}  // namespace alice_lamp