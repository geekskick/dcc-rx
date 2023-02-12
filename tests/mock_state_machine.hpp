#pragma once
#include "states/state_machine_interface.hpp"
#include <gmock/gmock.h>

struct MockStateMachine : public StateMachineInterface
{
    MOCK_METHOD(void, reset_to_waiting_preamble, (), (override));
    MOCK_METHOD(void, transition_to_collecting_data, (), (override));
    MOCK_METHOD(void, step, (const Microseconds &), (override));

    MOCK_METHOD(void, reset_buffer, (), (override));
    MOCK_METHOD(void, push_buffer, (uint8_t), (override));
    MOCK_METHOD(BufferInterface::BufferType &, get_buffer, (), (override));
};