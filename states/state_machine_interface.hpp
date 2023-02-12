#pragma once
#include "timestamp/microseconds.hpp"
#include "bits/buffer_interface.hpp"
struct StateMachineInterface{
    virtual void reset_to_waiting_preamble() = 0;
    virtual void transition_to_collecting_data() = 0;
    virtual void transition_to_packet_received() = 0;
    virtual void step(const Microseconds& pulse_width) = 0;
    virtual void reset_buffer() = 0;
    virtual void push_buffer(uint8_t) = 0;
    virtual void start_count() = 0;
    virtual size_t get_count_of_pushed_bits() const = 0;
    virtual BufferInterface::BufferType& get_buffer() = 0;
};