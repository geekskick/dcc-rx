#pragma once
#include "../gpio_state/gpio_state_interface.hpp"

using GpioNumber = int;

struct InputGpioInterface
{
    virtual GpioStateInterface::Level get() const = 0;
};

struct OutputGpioInterface
{
    virtual void set(const GpioStateInterface::Level &state) = 0;
    virtual const GpioStateInterface::Level &current_state() const = 0;
};