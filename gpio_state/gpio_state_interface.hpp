#pragma once
#include <ostream>
struct GpioStateInterface
{
    enum class Level
    {
        High,
        Low,
        Unknown
    };
    virtual const Level &current_level() const = 0;
    virtual void update(const Level &level) = 0;
};

std::ostream &operator<<(std::ostream &os, const GpioStateInterface::Level &level);

