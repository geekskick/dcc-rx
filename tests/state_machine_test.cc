#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bits/buffer_interface.hpp"
#include "states/state_machine.hpp"

struct MockState : public StateInterface
{
    MOCK_METHOD(void, run, (const Microseconds &, StateMachineInterface &), (override));
};

struct MockBuffer : public BufferInterface
{
    MOCK_METHOD(BufferInterface::BufferType &, data, (), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, push, (uint8_t), (override));
};

TEST(StateMachineTest, HasCorrectInitialState)
{
    auto buffer = testing::StrictMock<MockBuffer>{};
    auto initial = testing::NiceMock<MockState>{};
    auto other = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, other, buffer};
    EXPECT_CALL(initial, run(Microseconds{1}, testing::Ref(uut))).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsToCollectingState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto buffer = testing::StrictMock<MockBuffer>{};
    auto collecting_data = testing::NiceMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, buffer};
    uut.transition_to_collecting_data();
    EXPECT_CALL(collecting_data, run(Microseconds{1}, testing::Ref(uut))).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsBackToWaitingState)
{

    auto buffer = testing::StrictMock<MockBuffer>{};
    auto initial = testing::NiceMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, buffer};
    uut.transition_to_collecting_data();
    {
        testing::InSequence s;
        EXPECT_CALL(collecting_data, run(Microseconds{1}, testing::Ref(uut))).Times(1);
        EXPECT_CALL(initial, run(Microseconds{2}, testing::Ref(uut))).Times(1);
    }
    uut.step(Microseconds{1});
    uut.reset_to_waiting_preamble();
    uut.step(Microseconds(2));
}

TEST(StateMachineTest, ProvidesBuffer)
{
    auto s = testing::StrictMock<MockState>{};
    auto buffer = BufferInterface::BufferType{};
    auto b = testing::StrictMock<MockBuffer>{};
    auto uut = StateMachine{s, s, b};
    EXPECT_CALL(b, data()).Times(1).WillOnce(testing::ReturnRef(buffer));
    const auto &actual = uut.get_buffer();
    ASSERT_EQ(&actual, &buffer);
}
