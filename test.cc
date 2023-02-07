#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gpio/gpio_decorator.hpp"
#include "gpio_state/gpio_state.hpp"
#include "gpio_state/gpio_state_decorator.hpp"
#include "timestamp/timestamp_decorator.hpp"
#include "bits/bit_tolerance.hpp"
#include "bits/bit_factory.hpp"
#include "bits/bit_buffer.hpp"

TEST(GpioStateTest, ConstructedWithUnknownState)
{
    const auto uut = GpioState{};
    const auto expected = GpioState::Level::Unknown;
    ASSERT_EQ(uut.current_level(), expected);
}

TEST(GpioStateTest, Update)
{
    auto uut = GpioState{};
    const auto expected = GpioState::Level::High;
    uut.update(GpioState::Level::High);
    ASSERT_EQ(uut.current_level(), expected);
}
TEST(GpioStateTest, ConstructWithParam)
{
    auto uut = GpioState{GpioState::Level::High};
    const auto expected = GpioState::Level::High;
    ASSERT_EQ(uut.current_level(), expected);
}
TEST(GpioStateTest, Stringify)
{
    {
        std::stringstream ss;
        ss << GpioState::Level::High;
        ASSERT_STREQ("High", ss.str().c_str());
    }
    {
        std::stringstream ss;
        ss << GpioState::Level::Low;
        ASSERT_STREQ("Low", ss.str().c_str());
    }
    {
        std::stringstream ss;
        ss << GpioState::Level::Unknown;
        ASSERT_STREQ("Unknown", ss.str().c_str());
    }
}

struct GpioStateMock final : public GpioStateInterface
{
    MOCK_METHOD(void, update, (const GpioStateInterface::Level &), (override));
    MOCK_METHOD((const GpioStateInterface::Level &), current_level, (), (const, override));
};

TEST(GpioStateDecoratorTest, DelegatesCurrentLevelToWrappee)
{
    auto wrappee = GpioStateMock{};
    const auto uut = GpioStateDecorator{wrappee};
    const auto expected = GpioStateInterface::Level::Low;
    EXPECT_CALL(wrappee, current_level())
        .Times(1)
        .WillOnce(testing::ReturnRef(expected));
    ASSERT_EQ(expected, uut.current_level());
}

TEST(GpioStateDecoratorTest, DelegatesUpdateToWrappee)
{
    auto wrappee = GpioStateMock{};
    auto uut = GpioStateDecorator{wrappee};
    const auto expected = GpioStateInterface::Level::Low;
    EXPECT_CALL(wrappee, update(expected))
        .Times(1);
    uut.update(expected);
}

TEST(EdgeDetectingStateDecoratorTest, DetectsRisingEdge)
{
    auto wrappee = GpioState{GpioStateInterface::Level::Low};
    auto uut = EdgeDetectingGpioDecorator{wrappee};
    uut.update(GpioStateInterface::Level::High);
    const auto edge = uut.detect_edge();
    ASSERT_EQ(edge, EdgeDetectingGpioDecorator::Edge::Rising);
    ASSERT_EQ(uut.current_level(), GpioStateInterface::Level::High);
}
TEST(EdgeDetectingStateDecoratorTest, DetectsFallingEdge)
{
    auto wrappee = GpioState{GpioStateInterface::Level::High};
    auto uut = EdgeDetectingGpioDecorator{wrappee};
    uut.update(GpioStateInterface::Level::Low);
    const auto edge = uut.detect_edge();
    ASSERT_EQ(edge, EdgeDetectingGpioDecorator::Edge::Falling);
    ASSERT_EQ(uut.current_level(), GpioStateInterface::Level::Low);
}
TEST(EdgeDetectingStateDecoratorTest, DetectsNoEdge)
{
    auto wrappee = GpioState{GpioStateInterface::Level::Low};
    auto uut = EdgeDetectingGpioDecorator{wrappee};
    uut.update(GpioStateInterface::Level::Low);
    const auto edge = uut.detect_edge();
    ASSERT_EQ(edge, EdgeDetectingGpioDecorator::Edge::None);
    ASSERT_EQ(uut.current_level(), GpioStateInterface::Level::Low);
}
TEST(EdgeDetectingStateDecoratorTest, DetectsNoEdgeWhenUnitialised)
{
    const auto test = [](const GpioStateInterface::Level &level)
    {
        auto wrappee = GpioState{};
        auto uut = EdgeDetectingGpioDecorator{wrappee};
        uut.update(level);
        const auto edge = uut.detect_edge();
        ASSERT_EQ(edge, EdgeDetectingGpioDecorator::Edge::None);
        ASSERT_EQ(uut.current_level(), level);
    };
    test(GpioStateInterface::Level::Low);
    test(GpioStateInterface::Level::High);
}

struct OutputGpioMock final : public OutputGpioInterface
{
    MOCK_METHOD(void, set, (const GpioStateInterface::Level &), (override));
    MOCK_METHOD(const GpioStateInterface::Level &, current_state, (), (const override));
};

TEST(GpioDecoratorTest, DelegatesSetToWrapee)
{
    auto wrappee = OutputGpioMock{};
    auto uut = OutputGpioDecorator{wrappee};
    EXPECT_CALL(wrappee, set(GpioStateInterface::Level::High)).Times(1);
    uut.set(GpioStateInterface::Level::High);
}
TEST(GpioDecoratorTest, DelegatesGetToWrapee)
{
    auto wrappee = OutputGpioMock{};
    auto uut = OutputGpioDecorator{wrappee};
    const auto expected = GpioStateInterface::Level::High;
    EXPECT_CALL(wrappee, current_state()).Times(1).WillOnce(testing::ReturnRef(expected));
    const auto actual = uut.current_state();
    ASSERT_EQ(actual, expected);
}
TEST(TogglingGpioDecoratorTest, TogglesHighToLow){
    auto wrappee = OutputGpioMock{};
    auto uut = TogglingOutputGpioDecorator{wrappee};
    EXPECT_CALL(wrappee, current_state()).Times(1).WillOnce(testing::ReturnRefOfCopy(GpioStateInterface::Level::High));
    EXPECT_CALL(wrappee, set(GpioStateInterface::Level::Low)).Times(1);
    uut.toggle();
}
TEST(TogglingGpioDecoratorTest, TogglesLowToHigh)
{
    auto wrappee = OutputGpioMock{};
    auto uut = TogglingOutputGpioDecorator{wrappee};
    EXPECT_CALL(wrappee, current_state()).Times(1).WillOnce(testing::ReturnRefOfCopy(GpioStateInterface::Level::Low));
    EXPECT_CALL(wrappee, set(GpioStateInterface::Level::High)).Times(1);
    uut.toggle();
}

struct MockTimestamp : public TimestampInterface{
   MOCK_METHOD(const Microseconds&, get, (), (const override));
   MOCK_METHOD(void, update, (), (override));
   MOCK_METHOD(void, update, (const Microseconds&), (override));
};

TEST(PulseWidthDetectionTest, CanUpdateWithValue){
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

TEST(PulseWidthDetectionTest, PulseWidthIsCorrect){
    auto wrappee = testing::NiceMock<MockTimestamp>{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    EXPECT_CALL(wrappee, get())
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{1}))
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{2}));
    uut.update();
    const auto actual = uut.pulse_width();
    ASSERT_EQ(actual, Microseconds{1});
}
TEST(PulseWidthDetectionTest, PulseWidthIsCorrectAndPositive){
    auto wrappee = MockTimestamp{};
    auto uut = PulseWidthTimestampDecorator{wrappee};
    EXPECT_CALL(wrappee, get())
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{2}))
        .WillOnce(testing::ReturnRefOfCopy(Microseconds{1}));
    uut.update();
    const auto actual = uut.pulse_width();
    ASSERT_EQ(actual, Microseconds{1});
}

TEST(BitToleranceTest, InRange){
    const auto uut = BitTolerance{Microseconds{1}, Microseconds{3}};

    ASSERT_FALSE(uut.in_range(0));
    ASSERT_TRUE(uut.in_range(1));
    ASSERT_TRUE(uut.in_range(2));
    ASSERT_TRUE(uut.in_range(3));
    ASSERT_FALSE(uut.in_range(4));
}
TEST(BitFactoryTest, ReturnsNullOptWhenNotInTolerances){
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory{zero_tolerances, one_tolerances};
    const auto result = uut.create(Microseconds{4}); // OOL;
    ASSERT_FALSE(result.has_value());
}

TEST(BitFactoryTest, ReturnsZero)
{
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory{zero_tolerances, one_tolerances};
    const auto result = uut.create(Microseconds{6}); 
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 0);
}


TEST(BitFactoryTest, ReturnsOne)
{
    const auto zero_tolerances = BitTolerance{Microseconds{1}, Microseconds{3}};
    const auto one_tolerances = BitTolerance{Microseconds{5}, Microseconds{7}};
    const auto uut = BitFactory{zero_tolerances, one_tolerances};
    const auto result = uut.create(Microseconds{1}); 
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), 1);
}

TEST(BitBufferTest, InitialisedToZero){
    const auto uut = BitBuffer<2>{};
    ASSERT_EQ(uut.data.at(0), 0);
    ASSERT_EQ(uut.data.at(1), 0);
}