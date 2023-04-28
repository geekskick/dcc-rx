#pragma once
#include "bits/buffer_interface.hpp"
#include "timestamp/microseconds.hpp"
struct StateMachineInterface
{
    virtual void reset_to_waiting_preamble() = 0;
    virtual void transition_to_collecting_data() = 0;
    virtual void step(const Microseconds &pulse_width) = 0;
    virtual void reset_buffer() = 0;
    virtual void push_buffer(uint8_t) = 0;
    virtual const BufferInterface &get_buffer() const = 0;
};