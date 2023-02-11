#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "timestamp/timestamp_decorator.hpp"

struct MockTimestamp : public TimestampInterface
{
    MOCK_METHOD(const Microseconds &, get, (), (const override));
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(void, update, (const Microseconds &), (override));
};

TEST(PulseWidthDetectionTest, CanUpdateWithValue)
{
    auto wrappee = MockTimestamp{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    const auto stamp = Microseconds{100};
    EXPECT_CALL(wrappee, update(stamp)).Times(1);
    uut.update(stamp);
}

TEST(PulseWidthDetectionTest, CanUpdate)
{
    auto wrappee = MockTimestamp{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    const auto stamp = Microseconds{400};
    EXPECT_CALL(wrappee, get()).Times(1).WillOnce(testing::ReturnRef(stamp));
    EXPECT_CALL(wrappee, update()).Times(1);
    uut.update();
}

TEST(PulseWidthDetectionTest, PulseWidthIsCorrect)
{
    auto wrappee = testing::NiceMock<MockTimestamp>{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    EXPECT_CALL(wrappee, get())
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{1}))
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{2}));
    uut.update();
    const auto actual = uut.pulse_width();
    ASSERT_EQ(actual, Microseconds{1});
}
TEST(PulseWidthDetectionTest, PulseWidthIsCorrectAndPositive)
{
    auto wrappee = testing::NiceMock<MockTimestamp>{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    EXPECT_CALL(wrappee, get())
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{2}))
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{1}));
    uut.update();
    const auto actual = uut.pulse_width();
    ASSERT_EQ(actual, Microseconds{1});
}
