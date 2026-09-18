#include "alice_lamp/ring_animation.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace alice_lamp {
namespace {

using Palette = std::array<Color, 4>;

constexpr std::array<Palette, 4> palettes{{
    {{{15, 45, 255}, {85, 15, 210}, {180, 20, 255}, {20, 130, 255}}},
    {{{255, 18, 0}, {255, 80, 0}, {255, 170, 10}, {175, 0, 0}}},
    {{{255, 205, 0}, {150, 230, 10}, {15, 175, 45}, {235, 245, 15}}},
    {{{255, 20, 10}, {120, 0, 90}, {10, 55, 255}, {30, 155, 255}}},
}};

constexpr float pi = 3.14159265358979323846F;
constexpr std::size_t clock_animation_index = palettes.size();
constexpr std::size_t rainbow_animation_index = clock_animation_index + 1;
constexpr std::size_t animation_count = palettes.size() + 2;

Color blend(const Color& from, const Color& to, float amount) {
    const auto channel = [amount](std::uint8_t first, std::uint8_t second) {
        return static_cast<std::uint8_t>(first + (second - first) * amount);
    };
    return {channel(from.red, to.red), channel(from.green, to.green),
            channel(from.blue, to.blue)};
}

Color sample_palette(const Palette& palette, float position) {
    position -= std::floor(position);
    const float scaled = position * static_cast<float>(palette.size());
    const auto index = static_cast<std::size_t>(scaled) % palette.size();
    const std::size_t next = (index + 1) % palette.size();
    const float linear_mix = scaled - std::floor(scaled);
    const float smooth_mix = (1.0F - std::cos(linear_mix * pi)) * 0.5F;
    return blend(palette[index], palette[next], smooth_mix);
}

float circular_distance(float first, float second) {
    const float difference = std::fabs(first - second);
    return std::min(difference, 1.0F - difference);
}

float glow(float distance, float radius) {
    const float strength = std::max(0.0F, 1.0F - distance / radius);
    return strength * strength;
}

void add_light(Color& color, Color light, float strength) {
    const auto add_channel = [strength](std::uint8_t base, std::uint8_t added) {
        const auto result = static_cast<unsigned>(base) +
                            static_cast<unsigned>(added * strength);
        return static_cast<std::uint8_t>(std::min(result, 255U));
    };
    color = {add_channel(color.red, light.red),
             add_channel(color.green, light.green),
             add_channel(color.blue, light.blue)};
}

ColorSample sample_clock(std::uint32_t pixel_index, std::uint32_t pixel_count,
                         float rotation) {
    const float position = std::fmod(
        static_cast<float>(pixel_index) / pixel_count + 0.5F, 1.0F);
    const float pixel_radius = 1.35F / pixel_count;

    const float marker_position = std::round(position * 12.0F) / 12.0F;
    const float marker_strength = glow(
        circular_distance(position, marker_position), pixel_radius);
    const float twelve_strength = glow(circular_distance(position, 0.0F),
                                       pixel_radius * 1.8F);

    const float second_hand = rotation;
    const float minute_hand = std::fmod(rotation / 12.0F + 0.58F, 1.0F);
    const float hour_hand = std::fmod(rotation / 144.0F + 0.83F, 1.0F);

    Color color{3, 2, 12};
    add_light(color, {190, 110, 18}, marker_strength * 0.7F);
    add_light(color, {255, 220, 90}, twelve_strength);
    add_light(color, {70, 105, 255},
              glow(circular_distance(position, hour_hand), pixel_radius * 2.5F));
    add_light(color, {245, 235, 190},
              glow(circular_distance(position, minute_hand), pixel_radius * 1.8F));
    add_light(color, {255, 20, 12},
              glow(circular_distance(position, second_hand), pixel_radius * 1.25F));
    return {color, 1.0F};
}

ColorSample sample_rainbow(std::uint32_t pixel_index, std::uint32_t pixel_count,
                           float rotation) {
    const float position = static_cast<float>(pixel_index) / pixel_count + rotation;
    const float hue = position - std::floor(position);
    const float hue_sector = hue * 6.0F;
    const auto channel = [](float value) {
        return static_cast<std::uint8_t>(255.0F * std::clamp(value, 0.0F, 1.0F));
    };

    const Color color{
        channel(std::fabs(hue_sector - 3.0F) - 1.0F),
        channel(2.0F - std::fabs(hue_sector - 2.0F)),
        channel(2.0F - std::fabs(hue_sector - 4.0F)),
    };
    const float chase = 0.65F +
                        0.35F * (0.5F + 0.5F * std::sin(
                                                    6.0F * pi * position));
    return {color, chase};
}

}  // namespace

RingAnimation::RingAnimation(std::uint32_t pixel_count)
    : pixel_count_(pixel_count) {}

void RingAnimation::advance(float turns) {
    rotation_ += turns;
    rotation_ -= std::floor(rotation_);
}

void RingAnimation::next_animation() {
    animation_index_ = (animation_index_ + 1) % animation_count;
}

ColorSample RingAnimation::sample(std::uint32_t pixel_index) const {
    if (animation_index_ == clock_animation_index) {
        return sample_clock(pixel_index, pixel_count_, rotation_);
    }
    if (animation_index_ == rainbow_animation_index) {
        return sample_rainbow(pixel_index, pixel_count_, rotation_);
    }

    const float ring_position = static_cast<float>(pixel_index) / pixel_count_;
    const float position = ring_position + rotation_;
    const float intensity = 0.72F + 0.28F * std::sin(2.0F * pi * position);
    return {sample_palette(palettes[animation_index_], position), intensity};
}

}  // namespace alice_lamp