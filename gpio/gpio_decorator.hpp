#pragma once
#include "gpio_interface.hpp"
class OutputGpioDecorator : public OutputGpioInterface
{
protected:
    OutputGpioInterface &wrappee_;

public:
    OutputGpioDecorator(OutputGpioInterface &wrappee);
    void set(const GpioStateInterface::Level &state) override;
    const GpioStateInterface::Level &current_state() const override;
};

struct TogglingOutputGpioDecorator : public OutputGpioDecorator
{
    TogglingOutputGpioDecorator(OutputGpioInterface &wrappee);
    void toggle();
};

OutputGpioDecorator::OutputGpioDecorator(OutputGpioInterface &wrappee) : wrappee_{wrappee} {}

void OutputGpioDecorator::set(const GpioStateInterface::Level &state)
{
    wrappee_.set(state);
}
const GpioStateInterface::Level &OutputGpioDecorator::current_state() const
{
    return wrappee_.current_state();
}

TogglingOutputGpioDecorator::TogglingOutputGpioDecorator(OutputGpioInterface &wrappee) : OutputGpioDecorator{wrappee} {}
void TogglingOutputGpioDecorator::toggle()
{
    wrappee_.set(wrappee_.current_state() == GpioStateInterface::Level::High ? GpioStateInterface::Level::Low : GpioStateInterface::Level::High);
}
