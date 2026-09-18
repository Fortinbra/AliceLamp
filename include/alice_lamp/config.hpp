#pragma once

#include <cstdint>

#ifndef ALICE_LED_COUNT
#define ALICE_LED_COUNT 60
#endif

#ifndef ALICE_LED_PIN
#define ALICE_LED_PIN 2
#endif

#ifndef ALICE_POWER_TOUCH_PIN
#define ALICE_POWER_TOUCH_PIN 3
#endif

#ifndef ALICE_CYCLE_TOUCH_PIN
#define ALICE_CYCLE_TOUCH_PIN 4
#endif

namespace alice_lamp::config {

inline constexpr std::uint32_t led_count = ALICE_LED_COUNT;
inline constexpr std::uint32_t led_pin = ALICE_LED_PIN;
inline constexpr std::uint32_t power_touch_pin = ALICE_POWER_TOUCH_PIN;
inline constexpr std::uint32_t cycle_touch_pin = ALICE_CYCLE_TOUCH_PIN;
inline constexpr bool touch_active_high = true;
inline constexpr std::uint8_t brightness = 96;
inline constexpr std::uint32_t frame_interval_ms = 20;
inline constexpr std::uint32_t debounce_ms = 35;
inline constexpr float rotation_per_frame = 0.0025F;

static_assert(led_count > 0, "ALICE_LED_COUNT must be greater than zero");
static_assert(led_pin != power_touch_pin && led_pin != cycle_touch_pin &&
                  power_touch_pin != cycle_touch_pin,
              "LED and touch inputs must use different GPIO pins");

}  // namespace alice_lamp::config