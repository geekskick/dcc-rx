#pragma once
#include <cstdint>
#include "timestamp/microseconds.hpp"
#include "states/state_machine_interface.hpp"

struct StateMachineInterface;
struct StateInterface{
    virtual void run(const Microseconds& pulse_width) = 0;
    virtual void set_next_state(StateMachineInterface& state_machine) = 0;
};