#pragma once
#include "template_state.hpp"
#include <iostream>
/*
According to the standard:
    The preamble to a packet consists of a sequence of "1" bits.
    A digital decoder must not accept as a valid, any preamble
    that has less then 10 complete one bits, or require for
    proper reception of a packet with more than 12 complete one bits.
    A command station must send a minimum of 14 full preamble bits.

This is going to sit on a decoder, so the state is 10-12 ones followed by a
packet start bit (0).
*/
class WaitingForPreambleState final : public TemplateState
{
    int ones_received_{};
    bool move_to_next_state_{false};

    bool start_of_packet(const uint8_t bit) const
    {
        return bit == 0;
    }

    bool within_threshold() const
    {
        return ones_received_ >= MINIMUM_ONES;
    }

    void process_valid_bit(const uint8_t bit)
    {
        if (start_of_packet(bit) && within_threshold())
        {
            move_to_next_state_ = true;
            ones_received_ = 0;
        }
        else if (start_of_packet(bit))
        {
            ones_received_ = 0;
            move_to_next_state_ = false;
        }
        else if (!start_of_packet(bit))
        {
            ones_received_++;
            move_to_next_state_ = false;
        }
        std::cout << "Ones = " << ones_received_ << "\tmove_to_next = " << move_to_next_state_ << "\n";
    }

    void reset_self() override
    {
        move_to_next_state_ = false;
        ones_received_ = 0;
    }

    void template_run(const uint8_t bit, StateMachineInterface &state_machine) override
    {
        process_valid_bit(bit);
        if (move_to_next_state_)
        {
            state_machine.transition_to_collecting_data();
            state_machine.reset_buffer();
        }
    }

public:
    static constexpr size_t MINIMUM_ONES{10};

    WaitingForPreambleState(const BitFactory &bf) : TemplateState{bf} {}
};
