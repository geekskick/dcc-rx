#pragma once
#include "gpio_state_interface.hpp"
#include <array>
#include <string_view>

struct GpioState final : public GpioStateInterface
{
    GpioState() = default;
    GpioState(GpioStateInterface::Level level);
    const Level &current_level() const override;
    void update(const Level &level) override;

private:
    Level current_level_{GpioStateInterface::Level::Unknown};
};

GpioState::GpioState(GpioStateInterface::Level level) : current_level_{level} {};
const GpioState::Level &GpioState::current_level() const
{
    return current_level_;
}
void GpioState::update(const Level &level)
{
    current_level_ = level;
}

std::ostream &operator<<(std::ostream &os, const GpioStateInterface::Level &level)
{
    constexpr auto strings = std::array<std::string_view, 3>{"High", "Low", "Unknown"};
    using T = typename std::underlying_type_t<std::__remove_cvref_t<decltype(level)>>;
    const auto idx = static_cast<T>(level);
    os << strings.at(idx);
    return os;
}