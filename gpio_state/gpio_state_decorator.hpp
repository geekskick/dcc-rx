#pragma once
#include "gpio_state_interface.hpp"

class GpioStateDecorator : public GpioStateInterface
{
protected:
    GpioStateInterface &wrappee_;

public:
    GpioStateDecorator(GpioStateInterface &wrappee);
    virtual const GpioStateInterface::Level &current_level() const override;
    virtual void update(const GpioStateInterface::Level &level) override;
};

class EdgeDetectingGpioDecorator final : public GpioStateDecorator
{
private:
    GpioStateInterface::Level previous_level_;

public:
    enum class Edge
    {
        Rising,
        Falling,
        None
    };
    EdgeDetectingGpioDecorator(GpioStateInterface &wrappee);
    void update(const GpioStateInterface::Level &new_level) override;
    Edge detect_edge() const;
};

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
