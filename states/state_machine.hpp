#pragma once
#include "state_interface.hpp"
#include "state_machine_interface.hpp"
#include <functional>

class StateMachine : public StateMachineInterface
{
    std::reference_wrapper<StateInterface> current_state_;
    StateInterface &waiting_for_preamble_;
    StateInterface &collecting_data_;
    BufferInterface &buffer_;

public:
    StateMachine(StateInterface &initial_state, StateInterface &collecting_data, BufferInterface &buffer)
        : current_state_{initial_state},
          waiting_for_preamble_{initial_state},
          collecting_data_{collecting_data},
          buffer_{buffer} {}

    void reset_buffer() override
    {
        buffer_.reset();
    }

    void push_buffer(uint8_t bit) override
    {
        buffer_.push(bit);
    }

    const BufferInterface &get_buffer() const override
    {
        return buffer_;
    }
    void step(const Microseconds &pulse_width) override
    {
        current_state_.get().run(pulse_width, *this);
    }

    void reset_to_waiting_preamble() override
    {
        // std::cout << "Transitioning to waiting_for_preamble\n";
        current_state_ = waiting_for_preamble_;
    }

    void transition_to_collecting_data() override
    {
        // std::cout << "Transitioning to collecting_data\n";
        current_state_ = collecting_data_;
    }
};