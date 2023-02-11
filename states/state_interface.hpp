#pragma once
#include <cstdint>
#include "timestamp/microseconds.hpp"
struct StateInterface{
    virtual void run(const Microseconds& pulse_width) = 0;
};