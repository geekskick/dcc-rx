#pragma once

#include "template_state.hpp"
#include <iostream>

class CollectingDataState final : public TemplateState
{
    size_t bits_received_{};

    void template_run(const uint8_t bit, StateMachineInterface &sm) override
    {
        std::cout << "bit = " << bit << "\tbits received before this = " << bits_received_ << "\n";
        if (at_word_boundary() && packet_end(bit))
        {
            std::cout << "\tPacket rxd\n";
            sm.transition_to_packet_received();
        }
        else if (at_word_boundary() && !packet_end(bit))
        {
            // do nothing as we are expecting to see more bits
            std::cout << "\tAt a word boundary but expecting more data\n";
            bits_received_ = 0;
        }
        else
        {
            std::cout << "\tPushing to the buffer\n";
            sm.push_buffer(bit);
            bits_received_++;
        }
    }

    void reset_self() override
    {
        bits_received_ = 0;
    }

    bool packet_end(const uint8_t bit) const
    {
        return bit == PACKET_END_BIT;
    }

    bool at_word_boundary() const
    {
        return bits_received_ != 0 && bits_received_ % BITS_IN_WORD == 0;
    }

public:
    static constexpr size_t BITS_IN_WORD{8};
    static constexpr uint8_t DATA_START_BIT{1};
    static constexpr uint8_t PACKET_END_BIT{0};

    CollectingDataState(BitFactory &bf) : TemplateState{bf} {}
};