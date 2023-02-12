#pragma once

#include <array>
#include <cstddef> //size_t
#include <cstdint>

struct BufferInterface
{
    // 6 bytes should be enough fo now
    constexpr static size_t PACKET_BYTES_MAX = 6;
    using BufferType = std::array<uint8_t, PACKET_BYTES_MAX>;
    virtual BufferType &data() = 0;

    virtual void reset() = 0;
    virtual void push(uint8_t) = 0;
};