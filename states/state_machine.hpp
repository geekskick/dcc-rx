#pragma once
#include "state_interface.hpp"
#include "state_machine_interface.hpp"

class StateMachine : public StateMachineInterface
{
    std::reference_wrapper<StateInterface> current_state_;
    StateInterface &waiting_for_preamble_;
    StateInterface &collecting_data_;
    StateInterface &packet_received_;
    BufferInterface &buffer_;

public:
    StateMachine(StateInterface &initial_state, StateInterface &collecting_data, StateInterface &packet_received, BufferInterface &buffer)
        : current_state_{initial_state},
          waiting_for_preamble_{initial_state},
          collecting_data_{collecting_data},
          packet_received_{packet_received},
          buffer_{buffer} {}

    void reset_buffer() override
    {
        buffer_.reset();
    }

    void push_buffer(uint8_t bit) override
    {
        buffer_.push(bit);
    }
    void start_count() override
    {
        buffer_.start_count();
    }
    size_t get_count_of_pushed_bits() const override
    {
        return buffer_.get_count_of_pushed_bits();
    }

    BufferInterface::BufferType &get_buffer() override
    {
        return buffer_.data();
    }
    void step(const Microseconds &pulse_width) override
    {
        current_state_.get().run(pulse_width, *this);
    }

    void reset_to_waiting_preamble() override
    {
        std::cout << "Transitioning to waiting_for_preamble\n";
        current_state_ = waiting_for_preamble_;
    }

    void transition_to_collecting_data() override
    {
        std::cout << "Transitioning to collecting_data\n";
        current_state_ = collecting_data_;
    }

    void transition_to_packet_received() override
    {
        std::cout << "Transitioning to packet_received\n";
        current_state_ = packet_received_;
    }
};