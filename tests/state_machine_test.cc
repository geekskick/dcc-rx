#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bits/buffer_interface.hpp"
#include "states/state_machine.hpp"

struct MockState : public StateInterface
{
    MOCK_METHOD(void, run, (const Microseconds &), (override));
    MOCK_METHOD(void, visit, (StateMachineInterface &), (override));
};

struct MockBuffer : public BufferInterface
{
    MOCK_METHOD(BufferInterface::BufferType &, data, (), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, push, (uint8_t), (override));
    MOCK_METHOD(void, start_count, (), (override));
    MOCK_METHOD(size_t, get_count_of_pushed_bits, (), (const, override));
};

TEST(StateMachineTest, HasCorrectInitialState)
{
    auto buffer = testing::StrictMock<MockBuffer>{};
    auto initial = testing::NiceMock<MockState>{};
    auto other = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, other, other, buffer};
    EXPECT_CALL(initial, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsToCollectingState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto buffer = testing::StrictMock<MockBuffer>{};
    auto collecting_data = testing::NiceMock<MockState>{};
    auto pkt_rxd = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, pkt_rxd, buffer};
    uut.transition_to_collecting_data();
    EXPECT_CALL(collecting_data, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}
TEST(StateMachineTest, TranitionsToPacketReceviedState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto buffer = testing::StrictMock<MockBuffer>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::NiceMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved, buffer};
    uut.transition_to_packet_received();
    EXPECT_CALL(packet_recieved, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsBackToWaitingState)
{

    auto buffer = testing::StrictMock<MockBuffer>{};
    auto initial = testing::NiceMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::NiceMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved, buffer};
    uut.transition_to_packet_received();
    {
        testing::InSequence s;
        EXPECT_CALL(packet_recieved, run(Microseconds{1})).Times(1);
        EXPECT_CALL(initial, run(Microseconds{2})).Times(1);
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
    auto uut = StateMachine{s, s, s, b};
    EXPECT_CALL(b, data()).Times(1).WillOnce(testing::ReturnRef(buffer));
    const auto &actual = uut.get_buffer();
    ASSERT_EQ(&actual, &buffer);
}
/*
TEST(StateMachineTest, Visits){
    auto s = testing::StrictMock<MockState>{};
    auto uut = StateMachine{s, s, s};
    EXPECT_CALL(s, visit(uut)).Times(1);
    uut.update(s);
}*/