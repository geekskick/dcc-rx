#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bits/bit_factory.hpp"
#include "bits/bit_tolerance.hpp"

TEST(BitToleranceTest, InRange)
{
    const auto uut = BitTolerance{Microseconds{1}, Microseconds{3}};

    ASSERT_FALSE(uut.in_range(0));
    ASSERT_TRUE(uut.in_range(1));
    ASSERT_TRUE(uut.in_range(2));
    ASSERT_TRUE(uut.in_range(3));
    ASSERT_FALSE(uut.in_range(4));
}
TEST(BitFactoryTest, ReturnsNullOptWhenNotInTolerances)
{
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory::Builder{}.with_one_tolerance(one_tolerances).with_zero_tolerance(zero_tolerances).build();
    const auto result = uut.create(Microseconds{4}); // OOL;
    ASSERT_FALSE(result.has_value());
}

TEST(BitFactoryTest, ReturnsZero)
{
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory::Builder{}.with_one_tolerance(one_tolerances).with_zero_tolerance(zero_tolerances).build();
    const auto result = uut.create(Microseconds{2});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 0);
}

TEST(BitFactoryTest, ReturnsOne)
{
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory::Builder{}.with_one_tolerance(one_tolerances).with_zero_tolerance(zero_tolerances).build();
    const auto result = uut.create(Microseconds{6});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 1);
}
