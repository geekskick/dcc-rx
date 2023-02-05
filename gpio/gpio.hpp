#pragma once
#include "gpio_interface.hpp"

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
