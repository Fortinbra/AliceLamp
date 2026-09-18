#include "alice_lamp/ws2812_strip.hpp"

#include <algorithm>

#include "hardware/clocks.h"
#include "ws2812.pio.h"

namespace alice_lamp {
namespace {

constexpr float ws2812_frequency_hz = 800000.0F;

constexpr std::uint8_t gamma_correct(std::uint8_t channel) {
    const auto value = static_cast<std::uint32_t>(channel);
    return static_cast<std::uint8_t>((value * value + 127U) / 255U);
}

static_assert(gamma_correct(0) == 0);
static_assert(gamma_correct(128) == 64);
static_assert(gamma_correct(255) == 255);

std::uint32_t pack_grb(Color color, float intensity) {
    const auto scale_channel = [intensity](std::uint8_t channel) {
        const float safe_intensity = std::clamp(intensity, 0.0F, 1.0F);
        return static_cast<std::uint8_t>(gamma_correct(channel) * safe_intensity);
    };
    return (static_cast<std::uint32_t>(scale_channel(color.green)) << 24) |
           (static_cast<std::uint32_t>(scale_channel(color.red)) << 16) |
           (static_cast<std::uint32_t>(scale_channel(color.blue)) << 8);
}

}  // namespace

Ws2812Strip::Ws2812Strip(std::uint32_t data_pin)
    : data_pin_(data_pin),
      state_machine_(pio_claim_unused_sm(pio_, true)),
      program_offset_(pio_add_program(pio_, &ws2812_program)) {
    pio_gpio_init(pio_, data_pin_);
    pio_sm_set_consecutive_pindirs(pio_, state_machine_, data_pin_, 1, true);

    pio_sm_config state_machine_config =
        ws2812_program_get_default_config(program_offset_);
    sm_config_set_sideset_pins(&state_machine_config, data_pin_);
    sm_config_set_out_shift(&state_machine_config, false, true, 24);
    sm_config_set_fifo_join(&state_machine_config, PIO_FIFO_JOIN_TX);
    const float clock_divider =
        static_cast<float>(clock_get_hz(clk_sys)) /
        (ws2812_frequency_hz * (ws2812_T1 + ws2812_T2 + ws2812_T3));
    sm_config_set_clkdiv(&state_machine_config, clock_divider);
    pio_sm_init(pio_, state_machine_, program_offset_, &state_machine_config);
    pio_sm_set_enabled(pio_, state_machine_, true);
}

Ws2812Strip::~Ws2812Strip() {
    pio_sm_set_enabled(pio_, state_machine_, false);
    pio_sm_unclaim(pio_, state_machine_);
    pio_remove_program(pio_, &ws2812_program, program_offset_);
}

void Ws2812Strip::write(Color color, float intensity) {
    pio_sm_put_blocking(pio_, state_machine_, pack_grb(color, intensity));
}

void Ws2812Strip::clear(std::uint32_t pixel_count) {
    for (std::uint32_t pixel = 0; pixel < pixel_count; ++pixel) {
        pio_sm_put_blocking(pio_, state_machine_, 0);
    }
}

}  // namespace alice_lamp