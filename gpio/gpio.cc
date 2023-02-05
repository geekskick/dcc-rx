#include "hardware/gpio.h"
#include "gpio.hpp"

InputGpio::InputGpio(GpioNumber pin_number) : pin_{pin_number}
{
    gpio_init(pin_);
    gpio_set_dir(pin_, false);
}
GpioStateInterface::Level InputGpio::get() const
{
    return gpio_get(pin_) ? GpioStateInterface::Level::High : GpioStateInterface::Level::Low;
}

PullUpInputGpio::PullUpInputGpio(GpioNumber pin_number) : InputGpio(pin_number)
{
    gpio_set_pulls(pin_number, true, false);
}

PullDownInputGpio::PullDownInputGpio(GpioNumber pin_number) : InputGpio(pin_number)
{
    gpio_set_pulls(pin_number, false, true);
}

FloatingGpio::FloatingGpio(GpioNumber pin_number) : InputGpio{pin_number} {};

OutputGpio::OutputGpio(GpioNumber pin_number) : pin_{pin_number}, current_state_{GpioStateInterface::Level::Low}
{
    gpio_init(pin_);
    gpio_set_dir(pin_, true);
    gpio_put(pin_, false);
}
OutputGpio::OutputGpio(GpioNumber pin_number, GpioStateInterface::Level initial_state) : pin_{pin_number}, current_state_{initial_state}
{
    gpio_init(pin_);
    gpio_set_dir(pin_, true);
    gpio_put(pin_, initial_state == GpioStateInterface::Level::High);
}
void OutputGpio::set(const GpioStateInterface::Level &state)
{
    current_state_ = state;
    gpio_put(pin_, GpioStateInterface::Level::High == state);
}
const GpioStateInterface::Level &OutputGpio::current_state() const
{
    return current_state_;
}
