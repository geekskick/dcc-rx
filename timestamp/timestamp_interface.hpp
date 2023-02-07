#pragma once

#include "microseconds.hpp"
struct TimestampInterface{
    virtual const Microseconds& get() const = 0;
    virtual void update(const Microseconds& value) = 0;
    virtual void update() = 0;

};


