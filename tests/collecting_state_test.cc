#include "states/collecting_data.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(CollectingDataStateTest, SignalsOnValidEndOfData)
{
    auto bf = BitFactory::Builder{}.with_zero_tolerance(BitTolerance{1, 3}).with_one_tolerance(BitTolerance{4, 6}).build();
    auto uut = CollectingDataState{bf};
}