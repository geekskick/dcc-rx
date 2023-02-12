#pragma once

struct BufferInterface{
    // 6 bytes should be enough fo now
    constexpr static size_t PACKET_BYTES_MAX = 6;
    using BufferType = std::array<uint8_t, PACKET_BYTES_MAX>;
    virtual void push(uint8_t) = 0;
    virtual const BufferType& data() const = 0;
};