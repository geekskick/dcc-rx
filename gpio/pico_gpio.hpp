#pragma once
#include "gpio_interface.hpp"
#include "hardware/gpio.h"
class InputGpio : public InputGpioInterface
{
protected:
    InputGpio(GpioNumber pin_number);
public:
    GpioStateInterface::Level get() const override;
    
private:
    GpioNumber pin_;
};

struct PullUpInputGpio : public InputGpio
{
    PullUpInputGpio(GpioNumber pin_number);
};

struct PullDownInputGpio : public InputGpio
{
    PullDownInputGpio(GpioNumber pin_number) ;
};

struct FloatingGpio : public InputGpio
{
    FloatingGpio(GpioNumber pin_number);
};

struct OutputGpio : public OutputGpioInterface
{
public:
    OutputGpio(GpioNumber pin_number);
    OutputGpio(GpioNumber pin_number, GpioStateInterface::Level initial_state);
    void set(const GpioStateInterface::Level &state) override;
    const GpioStateInterface::Level &current_state() const override;

private:
    GpioNumber pin_;
    GpioStateInterface::Level current_state_;
};


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
