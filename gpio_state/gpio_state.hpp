#pragma once
#include "gpio_state_interface.hpp"
struct GpioState final : public GpioStateInterface
{
    GpioState() = default;
    GpioState(GpioStateInterface::Level level);
    const Level &current_level() const override;
    void update(const Level &level) override;

private:
    Level current_level_{GpioStateInterface::Level::Unknown};
};
