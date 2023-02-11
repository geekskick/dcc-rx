#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "states/state_machine.hpp"

struct MockState : public StateInterface
{
    MOCK_METHOD(void, run, (const Microseconds &), (override));
};

TEST(StateMachineTest, HasCorrectInitialState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial};
    EXPECT_CALL(initial, run(Microseconds{1})).Times(1);
    uut.run(Microseconds{1});
}

TEST(StateMachineTest, TranitionsToCollectingState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial};
    uut.transition_to(collecting_data);
    EXPECT_CALL(collecting_data, run(Microseconds{1})).Times(1);
    uut.run(Microseconds{1});
}
TEST(StateMachineTest, TranitionsToPacketReceviedState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved};
    uut.transition_to_packet_received();
    EXPECT_CALL(packet_recieved, run(Microseconds{1})).Times(1);
    uut.run(Microseconds{1});
}

TEST(StateMachineTest, TranitionsBackToWaitingState)
{
    auto initial = testing::StrictMock<MockState>{};
    auto collecting_data = testing::StrictMock<MockState>{};
    auto packet_recieved = testing::StrictMock<MockState>{};
    auto uut = StateMachine{initial, collecting_data, packet_recieved};
    uut.transition_to_packet_received();
    {
        testing::InSequence s;
        EXPECT_CALL(packet_recieved, run(Microseconds{1})).Times(1);
        EXPECT_CALL(initial, run(Microseconds{2})).Times(1);
    }
    uut.run(Microseconds{1});
    uut.reset_to_waiting_preamble();
    uut.run(Microseconds(2));
}