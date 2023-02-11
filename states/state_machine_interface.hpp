#pragma once
#include "state_interface.hpp"
#include "timestamp/microseconds.hpp"
struct StateMachineInterface{
    virtual void reset_to_waiting_preamble() = 0;
    virtual void transition_to_collecting_data() = 0;
    virtual void transition_to_packet_received() = 0;
    virtual void run(const Microseconds& pulse_width) = 0;
};