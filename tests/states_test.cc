#include "states/waiting_for_preamble.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
/*
The preamble to a packet consists of a sequence of "1" bits.
A digital decoder must not accept as a valid, any preamble that
has less then 10 complete one bits, or require for proper reception
of a packet with more than 12 complete one bits. A command
station must send a minimum of 14 full preamble bits.
*/

struct MockStateMachine : public StateMachineInterface
{
    MOCK_METHOD(void, reset_to_waiting_preamble, (), (override));
    MOCK_METHOD(void, transition_to_collecting_data, (), (override));
    MOCK_METHOD(void, transition_to_packet_received, (), (override));
    MOCK_METHOD(void, step, (const Microseconds &), (override));

    MOCK_METHOD(void, reset_buffer, (), (override));
    MOCK_METHOD(void, push_buffer, (uint8_t), (override));
    MOCK_METHOD(void, start_count, (), (override));
    MOCK_METHOD(size_t, get_count_of_pushed_bits, (), (const, override));
    MOCK_METHOD(BufferInterface::BufferType &, get_buffer, (), (override));
};

TEST(WaitingForPremableState, TransitionsOnCorrectPreambleAtMinimumLimit)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    auto sm = testing::StrictMock<MockStateMachine>{};
    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1});
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.visit(sm);
}

TEST(WaitingForPremableState, ResetsCountWhenInvalidBitReceivedBeforePacket)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    constexpr auto min_ones = 10;
    auto sm = testing::StrictMock<MockStateMachine>{};

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES - 1; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // Invalid bit so we dont expect a transition request on visit
    uut.run(Microseconds{10});
    uut.visit(sm);

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // Start of packet bit
    uut.run(Microseconds{1});

    // Now we expect a call
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.visit(sm);
}

TEST(WaitingForPremableState, ResetsCountWhenInvalidBitReceivedInValidWindow)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    auto sm = testing::StrictMock<MockStateMachine>{};

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // Invalid bit so we dont expect a transition request on visit
    uut.run(Microseconds{10});
    uut.visit(sm);

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // Start of packet bit
    uut.run(Microseconds{1});

    // Now we expect a call
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.visit(sm);
}

TEST(WaitingForPremableState, TransitionsOnCorrectPreambleAtMaximumLimit)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    auto sm = testing::StrictMock<MockStateMachine>{};
    for (int i = 0; i < WaitingForPreambleState::MAXIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1});
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.visit(sm);
}
TEST(WaitingForPremableState, DoesntTransitionsOnCorrectPreambleAboveMaximumLimit)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    constexpr auto max_ones = 12;
    auto sm = testing::StrictMock<MockStateMachine>{};
    for (int i = 0; i < WaitingForPreambleState::MAXIMUM_ONES + 1; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1});
    uut.visit(sm);
}
TEST(WaitingForPremableState, DoesntTransitionsOnCorrectPreambleBelowMinimumLimit)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    auto sm = testing::StrictMock<MockStateMachine>{};
    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES - 1; ++i)
    {
        uut.run(Microseconds{5});
        uut.visit(sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1});
    uut.visit(sm);
}