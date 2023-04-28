#pragma once
#include "buffer_interface.hpp"
#include <cstdio>
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
    const BufferType &data() const { return data_; }

    void reset() override
    {
        data_.fill(BufferType::value_type{});
        current_bit = 7;
        current_byte = 0;
        // std::cout << "buffer reset\n";
    }

    void push(uint8_t bit) override
    {
        const auto bit_masked = 0x01 & bit;
        const auto bit_shifted = bit_masked << current_bit;
        data_.at(current_byte) |= bit_shifted;
        increment_indexes();
        // printf("Buffer = %02X\tbit=%d\tbyte=%d\n", data_[0], current_bit, current_byte);
        //  std::cout << "Buffer = " << std::hex << std::setw(2) << std::setfill('0') << data_.at(0)
        //            << "\n\tcurrent_bit = " << std::dec << current_bit << " current byte = " << current_byte << "\n";
    }

    int bit() const
    {
        return current_bit;
    }

    int byte() const
    {
        return current_byte;
    }
};