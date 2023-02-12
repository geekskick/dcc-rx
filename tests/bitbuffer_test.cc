#include <gtest/gtest.h>

#include "bits/bit_buffer.hpp"

TEST(BitBufferTest, InitialisedToZero)
{
    auto uut = BitBuffer{};
    for (const auto &b : uut.data())
    {
        ASSERT_EQ(b, 0);
    }
}

TEST(BitBufferTest, Pushes)
{
    auto uut = BitBuffer{};
    uut.push(1);
    ASSERT_EQ(0x80, uut.data().at(0));
}
TEST(BitBufferTest, PushesTwice)
{
    auto uut = BitBuffer{};
    uut.push(1);
    uut.push(1);
    ASSERT_EQ(0xC0, uut.data().at(0));
}
TEST(BitBufferTest, PushesIntoNextByte)
{
    auto uut = BitBuffer{};
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    uut.push(1);
    ASSERT_EQ(0xff, uut.data().at(0));
    ASSERT_EQ(0x80, uut.data().at(1));
}
TEST(BitBufferTest, Resets)
{
    auto uut = BitBuffer{};
    uut.push(1);
    uut.reset();
    for (const auto &b : uut.data())
    {
        ASSERT_EQ(b, 0);
    }
}