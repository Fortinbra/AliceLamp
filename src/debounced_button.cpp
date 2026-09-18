#include "alice_lamp/debounced_button.hpp"

#include "pico/stdlib.h"

namespace alice_lamp {

DebouncedButton::DebouncedButton(std::uint32_t pin, bool active_high,
                                 std::uint32_t debounce_ms)
    : pin_(pin),
      debounce_ms_(debounce_ms),
      changed_at_ms_(to_ms_since_boot(get_absolute_time())),
      active_high_(active_high),
      raw_state_(false),
      stable_state_(false) {
    gpio_init(pin_);
    gpio_set_dir(pin_, GPIO_IN);
    active_high_ ? gpio_pull_down(pin_) : gpio_pull_up(pin_);
    raw_state_ = is_pressed();
    stable_state_ = raw_state_;
}

bool DebouncedButton::pressed(std::uint32_t now_ms) {
    const bool current = is_pressed();
    if (current != raw_state_) {
        raw_state_ = current;
        changed_at_ms_ = now_ms;
    }

    if (current != stable_state_ && now_ms - changed_at_ms_ >= debounce_ms_) {
        stable_state_ = current;
        return stable_state_;
    }
    return false;
}

bool DebouncedButton::is_pressed() const {
    return static_cast<bool>(gpio_get(pin_)) == active_high_;
}

}  // namespace alice_lamp