#pragma once
#include "gpio_interface.hpp"
class OutputGpioDecorator : public OutputGpioInterface
{
protected:
    OutputGpioInterface &wrappee_;
    OutputGpioDecorator(OutputGpioInterface &wrappee);

public:
    void set(const GpioStateInterface::Level &state) override;
    const GpioStateInterface::Level &current_state() const override;
};

struct TogglingOutputGpioDecorator : public OutputGpioDecorator
{
    TogglingOutputGpioDecorator(OutputGpioInterface &wrappee);
    void toggle();
};