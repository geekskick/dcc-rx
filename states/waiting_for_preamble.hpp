#pragma once
#include "state_interface.hpp"
#include "bits/bit_factory.hpp"
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
class WaitingForPreambleState final : public StateInterface{
    int ones_received_{};
    bool move_to_next_state_{false};
    BitFactory& bit_factory_;
    
    bool start_of_packet(const uint8_t bit) const {
        return bit == 0;
    }

    bool within_threshold() const {
        return ones_received_ >= MINIMUM_ONES && ones_received_ <= MAXIMUM_ONES;
    }

    void process_valid_bit(const uint8_t bit) {
        if(start_of_packet(bit) && within_threshold()){
            move_to_next_state_ = true;
            ones_received_ = 0;
        }
        else if(start_of_packet(bit)){
            ones_received_ = 0;
            move_to_next_state_ = false;
        }
        else if(!start_of_packet(bit)){
            ones_received_++;
            move_to_next_state_ = false;
        }
    }


public:
    static constexpr size_t MINIMUM_ONES{10};
    static constexpr size_t MAXIMUM_ONES{12};
    void visit(StateMachineInterface &state_machine) override
    {
        if (move_to_next_state_)
        {
            state_machine.transition_to_collecting_data();
            state_machine.reset_buffer();
        }
    }

    WaitingForPreambleState(BitFactory& bf):bit_factory_{bf}{}
    void run(const Microseconds& pulse_width) override{
        const auto bit = bit_factory_.create(pulse_width);
        if(bit){
            process_valid_bit(bit.value());
        }
        else{
            ones_received_ = 0;
        }
    }
};

