#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "states/state_machine.hpp"

struct MockState : public StateInterface
{
    MOCK_METHOD(void, run, (const Microseconds &), (override));
    MOCK_METHOD(void, set_next_state, (StateMachineInterface&), (override));
};

TEST(StateMachineTest, HasCorrectInitialState)
{
    auto initial = testing::NiceMock<MockState>{};
    auto other = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, other, other};
    EXPECT_CALL(initial, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsToCollectingState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto collecting_data = testing::NiceMock<MockState>{};
    auto pkt_rxd = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, pkt_rxd};
    uut.transition_to_collecting_data();
    EXPECT_CALL(collecting_data, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}
TEST(StateMachineTest, TranitionsToPacketReceviedState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::NiceMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved};
    uut.transition_to_packet_received();
    EXPECT_CALL(packet_recieved, run(Microseconds{1})).Times(1);
    uut.step(Microseconds{1});
}

TEST(StateMachineTest, TranitionsBackToWaitingState)
{
    auto initial = testing::NiceMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::NiceMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved};
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
/*
TEST(StateMachineTest, Visits){
    auto s = testing::StrictMock<MockState>{};
    auto uut = StateMachine{s, s, s};
    EXPECT_CALL(s, visit(uut)).Times(1);
    uut.update(s);
}*/