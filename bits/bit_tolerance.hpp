#pragma once
#include "timestamp/microseconds.hpp"

struct BitTolerance{
    Microseconds low{};
    Microseconds high{};
    bool in_range(const Microseconds& time) const {
        return time >= low && time <= high;
    }
};