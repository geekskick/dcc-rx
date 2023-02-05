#include "gpio_state_decorator.hpp"

GpioStateDecorator::GpioStateDecorator(GpioStateInterface &wrappee) : wrappee_{wrappee} {};
const GpioStateInterface::Level &GpioStateDecorator::current_level() const
{
    return wrappee_.current_level();
}
void GpioStateDecorator::update(const GpioStateInterface::Level &level)
{
    wrappee_.update(level);
}

EdgeDetectingGpioDecorator::EdgeDetectingGpioDecorator(GpioStateInterface &wrappee) : GpioStateDecorator{wrappee}, previous_level_{GpioStateInterface::Level::Unknown} {};
void EdgeDetectingGpioDecorator::update(const GpioStateInterface::Level &new_level)
{
    previous_level_ = wrappee_.current_level();
    wrappee_.update(new_level);
}

EdgeDetectingGpioDecorator::Edge EdgeDetectingGpioDecorator::detect_edge() const
{
    if (previous_level_ == GpioStateInterface::Level::Low &&
        wrappee_.current_level() == GpioStateInterface::Level::High)
    {
        return Edge::Rising;
    }
    else if (previous_level_ == GpioStateInterface::Level::High &&
             wrappee_.current_level() == GpioStateInterface::Level::Low)
    {
        return Edge::Falling;
    }
    else
    {
        return Edge::None;
    }
}
