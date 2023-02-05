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
