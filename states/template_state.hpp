#pragma once
#include "bits/bit_factory.hpp"
#include "state_interface.hpp"
#include "state_machine_interface.hpp"

struct TemplateState : public StateInterface
{

    void run(const Microseconds &pulse_width, StateMachineInterface &sm) override
    {
        const auto bit = bit_factory_.create(pulse_width);
        if (bit)
        {
            template_run(bit.value(), sm);
        }
        else
        {
            reset_self();
            sm.reset_buffer();
            sm.reset_to_waiting_preamble();
        }
    }

protected:
    virtual void reset_self() = 0;
    virtual void template_run(const uint8_t bit, StateMachineInterface &state_machine) = 0;
    TemplateState(BitFactory &bf) : bit_factory_{bf} {}

private:
    BitFactory &bit_factory_;
};