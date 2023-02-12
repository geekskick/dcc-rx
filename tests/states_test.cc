#include "states/collecting_data.hpp"
#include "states/waiting_for_preamble.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
        uut.run(Microseconds{5}, sm);
    }
    // start of packet is the end of the preamble
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.run(Microseconds{1}, sm);
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
        uut.run(Microseconds{5}, sm);
    }
    // Invalid bit so we dont expect a transition request on visit
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    EXPECT_CALL(sm, reset_to_waiting_preamble()).Times(1);
    uut.run(Microseconds{10}, sm);

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5}, sm);
    }
    // Start of packet bit
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.run(Microseconds{1}, sm);
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
        uut.run(Microseconds{5}, sm);
    }
    // Invalid bit so we dont expect a transition request on visit
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    EXPECT_CALL(sm, reset_to_waiting_preamble()).Times(1);
    uut.run(Microseconds{10}, sm);

    for (int i = 0; i < WaitingForPreambleState::MINIMUM_ONES; ++i)
    {
        uut.run(Microseconds{5}, sm);
    }
    // Start of packet bit
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.run(Microseconds{1}, sm);
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
        uut.run(Microseconds{5}, sm);
    }
    // start of packet is the end of the preamble
    EXPECT_CALL(sm, transition_to_collecting_data()).Times(1);
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    uut.run(Microseconds{1}, sm);
}
TEST(WaitingForPremableState, DoesntTransitionsOnCorrectPreambleAboveMaximumLimit)
{
    // 1,2,3 are a 0,
    // 4,5,6 are a 1
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = WaitingForPreambleState{bf};
    auto sm = testing::StrictMock<MockStateMachine>{};
    for (int i = 0; i < WaitingForPreambleState::MAXIMUM_ONES + 1; ++i)
    {
        uut.run(Microseconds{5}, sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1}, sm);
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
        uut.run(Microseconds{5}, sm);
    }
    // start of packet is the end of the preamble
    uut.run(Microseconds{1}, sm);
}