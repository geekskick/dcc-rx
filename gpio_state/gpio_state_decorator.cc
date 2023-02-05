#include "gpio_state_decorator.hpp"
#include <iostream>

GpioStateDecorator::GpioStateDecorator(GpioStateInterface &wrappee) : wrappee_{wrappee} {};
const GpioStateInterface::Level &GpioStateDecorator::current_level() const
{
    return wrappee_.current_level();
}
void GpioStateDecorator::update(const GpioStateInterface::Level &level)
{
    wrappee_.update(level);
}

EdgeDetectingGpio::EdgeDetectingGpio(GpioStateInterface &wrappee) : GpioStateDecorator{wrappee}, previous_level_{GpioStateInterface::Level::Unknown} {};
void EdgeDetectingGpio::update(const GpioStateInterface::Level &new_level)
{
    previous_level_ = wrappee_.current_level();
    wrappee_.update(new_level);
}

EdgeDetectingGpio::Edge EdgeDetectingGpio::detect_edge() const
{
    static int print = 0;
    const auto indicator_character = print++ & 0x01 == 0x01 ? "-" : "|";
     std::cout << indicator_character << "\tdetecting_edge [" << previous_level_ << "," << wrappee_.current_level() << "]" << std::endl;
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
