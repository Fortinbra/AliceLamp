#include <cstdint>

#include "alice_lamp/config.hpp"
#include "alice_lamp/debounced_button.hpp"
#include "alice_lamp/ring_animation.hpp"
#include "alice_lamp/ws2812_strip.hpp"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();

    alice_lamp::Ws2812Strip strip(alice_lamp::config::led_pin);
    alice_lamp::DebouncedButton power_button(
        alice_lamp::config::power_touch_pin,
        alice_lamp::config::touch_active_high,
        alice_lamp::config::debounce_ms);
    alice_lamp::DebouncedButton cycle_button(
        alice_lamp::config::cycle_touch_pin,
        alice_lamp::config::touch_active_high,
        alice_lamp::config::debounce_ms);
    alice_lamp::RingAnimation animation(alice_lamp::config::led_count);

    bool lights_on = true;
    std::uint32_t last_frame_ms = 0;

    while (true) {
        const std::uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if (power_button.pressed(now_ms)) {
            lights_on = !lights_on;
        }
        if (cycle_button.pressed(now_ms)) {
            animation.next_animation();
        }

        if (now_ms - last_frame_ms >= alice_lamp::config::frame_interval_ms) {
            last_frame_ms = now_ms;
            if (lights_on) {
                animation.advance(alice_lamp::config::rotation_per_frame);
                for (std::uint32_t pixel = 0;
                     pixel < alice_lamp::config::led_count; ++pixel) {
                    const auto sample = animation.sample(pixel);
                    const float brightness = sample.intensity *
                                             alice_lamp::config::brightness / 255.0F;
                    strip.write(sample.color, brightness);
                }
            } else {
                strip.clear(alice_lamp::config::led_count);
            }
        }

        sleep_ms(1);
    }
}