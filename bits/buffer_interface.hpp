#pragma once

#include <array>
#include <cstddef> //size_t

struct BufferInterface{
    // 6 bytes should be enough fo now
    constexpr static size_t PACKET_BYTES_MAX = 6;
    using BufferType = std::array<uint8_t, PACKET_BYTES_MAX>;
    virtual BufferType& data() = 0;

    virtual void reset() = 0;
    virtual void push(uint8_t) = 0;
    virtual void start_count() = 0;
    virtual size_t get_count_of_pushed_bits() const = 0;
};