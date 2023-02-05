#include "gpio_decorator.hpp"
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
