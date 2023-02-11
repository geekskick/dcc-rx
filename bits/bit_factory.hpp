#pragma once

#include "bit_tolerance.hpp"
#include "timestamp/microseconds.hpp"
#include <optional>

class BitFactory
{
    BitTolerance zero{};
    BitTolerance one{};

    BitFactory(const BitTolerance zero_tolerance, const BitTolerance one_tolerance)
        : zero{std::move(zero_tolerance)},
          one{std::move(one_tolerance)}
    {
    }
public:
    class Builder{
        BitTolerance one_{};
        BitTolerance zero_{};
        public:
        Builder& with_one_tolerance(const BitTolerance& one){
            one_ = one;
            return *this;
        }
        Builder& with_zero_tolerance(const BitTolerance& zero){
            zero_ = zero;
            return *this;
        }

        BitFactory build() const {
            return BitFactory{zero_, one_};
        }
    };

    std::optional<uint8_t> create(const Microseconds &pulse_width) const
    {
        if (one.in_range(pulse_width))
        {
            return 1;
        }
        else if (zero.in_range(pulse_width))
        {
            return 0;
        }
        else
        {
            return std::nullopt;
        }
    }
};