#pragma once
#include "state_interface.hpp"
#include "bits/bit_factory.hpp"
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
    int minimum_ones_{10};
    int maximum_ones_{12};
    BitFactory& bit_factory_;
    bool start_of_packet(const uint8_t bit) const {
        return bit == 0;
    }

    bool within_threshold() const {
        return ones_received_ >= minimum_ones_ && ones_received_ <= maximum_ones_;
    }

    void process_valid_bit(const uint8_t bit) {
        if(start_of_packet(bit) && within_threshold()){
            
        }
    }
public:
    WaitingForPreambleState(BitFactory& bf):bit_factory_{bf}{}
    void run(const Microseconds& pulse_width) override{
        const auto bit = bit_factory_.create(pulse_width);
        if(bit){
            process_valid_bit(bit);
        }
    }
};