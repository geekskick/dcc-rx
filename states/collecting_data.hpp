#pragma once

#include "template_state.hpp"

class CollectingDataState final : public TemplateState
{
    size_t bits_received_{};

    void template_run(const uint8_t bit, StateMachineInterface &sm) override
    {
    }

    void reset_self() override
    {
        bits_received_ = 0;
    }

public:
    static constexpr size_t BITS_IN_WORD{8};
    static constexpr uint8_t DATA_START_BIT{1};
    static constexpr uint8_t PACKET_END_BIT{0};
};