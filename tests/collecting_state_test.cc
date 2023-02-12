#include "mock_state_machine.hpp"
#include "states/collecting_data.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(CollectingDataStateTest, SignalsOnValidEndOfData)
{
    auto called = 0;
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = CollectingDataState{bf, [&](const BufferInterface::BufferType &b)
                                   {
                                       called++;
                                   }};
    auto sm = testing::StrictMock<MockStateMachine>{};

    EXPECT_CALL(sm, push_buffer(0)).Times(uut.BITS_IN_WORD);

    for (int i = 0; i < uut.BITS_IN_WORD; i++)
    {
        uut.run(Microseconds{1}, sm);
    }

    EXPECT_CALL(sm, get_buffer()).Times(1).WillOnce(testing::ReturnRefOfCopy(BufferInterface::BufferType{}));
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    EXPECT_CALL(sm, reset_to_waiting_preamble()).Times(1);
    uut.run(Microseconds{1}, sm);
    ASSERT_EQ(called, 1);
}

TEST(CollectingDataStateTest, SignalsOnValidEndOfDataAfterTwoWords)
{
    auto called = 0;
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = CollectingDataState{bf, [&](const BufferInterface::BufferType &b)
                                   { called++; }};
    auto sm = testing::StrictMock<MockStateMachine>{};

    EXPECT_CALL(sm, push_buffer(0)).Times(uut.BITS_IN_WORD * 2);

    for (int i = 0; i < uut.BITS_IN_WORD; i++)
    {
        uut.run(Microseconds{1}, sm);
    }

    uut.run(Microseconds{5}, sm); // another word incoming

    for (int i = 0; i < uut.BITS_IN_WORD; i++)
    {
        uut.run(Microseconds{1}, sm);
    }

    EXPECT_CALL(sm, get_buffer()).Times(1).WillOnce(testing::ReturnRefOfCopy(BufferInterface::BufferType{}));
    EXPECT_CALL(sm, reset_buffer()).Times(1);
    EXPECT_CALL(sm, reset_to_waiting_preamble()).Times(1);
    uut.run(Microseconds{1}, sm);
    ASSERT_EQ(called, 1);
}

TEST(CollectingDataStateTest, ResetsOnInvalidBitMidWord)
{
    auto called = false;
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = CollectingDataState{bf, [&](const BufferInterface::BufferType &b)
                                   { called = true; }};
    auto sm = testing::StrictMock<MockStateMachine>{};

    EXPECT_CALL(sm, push_buffer(0)).Times(uut.BITS_IN_WORD - 2);

    for (int i = 0; i < uut.BITS_IN_WORD - 2; i++)
    {
        uut.run(Microseconds{1}, sm);
    }

    EXPECT_CALL(sm, reset_buffer()).Times(1);
    EXPECT_CALL(sm, reset_to_waiting_preamble()).Times(1);
    uut.run(Microseconds{10}, sm);
    ASSERT_FALSE(called);
}