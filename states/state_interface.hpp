#pragma once
#include "states/state_machine_interface.hpp"
#include "timestamp/microseconds.hpp"
#include <cstdint>

struct StateMachineInterface;
struct StateInterface
{
    virtual void run(const Microseconds &pulse_width, StateMachineInterface &sm) = 0;
};