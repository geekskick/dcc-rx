#pragma once
#include "buffer_interface.hpp"

class BitBuffer final : public BufferInterface
{
    BufferType data_{};
    int current_bit{7}; // purposely signed
    size_t current_byte{};

    void increment_indexes()
    {
        current_bit--;
        current_byte += current_bit < 0 ? 1 : 0;
        current_bit = current_bit < 0 ? 7 : current_bit;
    }

public:
    BufferType &data() { return data_; }

    void reset() override
    {
        data_.fill(BufferType::value_type{});
    }

    void push(uint8_t bit) override
    {
        const auto bit_masked = 0x01 & bit;
        const auto bit_shifted = bit_masked << current_bit;
        data_.at(current_byte) |= bit_shifted;
        increment_indexes();
    }
};