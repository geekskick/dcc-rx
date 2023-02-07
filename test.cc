#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gpio_state/gpio_state.hpp"
#include "gpio_state/gpio_state_decorator.hpp"
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
