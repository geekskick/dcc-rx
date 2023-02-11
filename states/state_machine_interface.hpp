#pragma once
#include "timestamp/microseconds.hpp"
struct StateMachineInterface{
    virtual void reset_to_waiting_preamble() = 0;
    virtual void transition_to_collecting_data() = 0;
    virtual void transition_to_packet_received() = 0;
    virtual void step(const Microseconds& pulse_width) = 0;
};