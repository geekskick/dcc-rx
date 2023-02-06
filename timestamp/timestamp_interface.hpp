#pragma once

#include <cstdint>

using Microseconds = uint64_t;

struct TimestampInterface{
    virtual const Microseconds& get() const = 0;
    virtual void update(const Microseconds& value) = 0;
    virtual void update() = 0;

};


