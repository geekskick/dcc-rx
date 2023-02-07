#pragma once

#include "bit_tolerance.hpp"
#include "timestamp/microseconds.hpp"
#include <optional>

class BitFactory
{
    BitTolerance zero{};
    BitTolerance one{};

public:
    BitFactory(const BitTolerance zero_tolerance, const BitTolerance one_tolerance)
        : zero{std::move(one_tolerance)},
          one{std::move(zero_tolerance)}
    {
    }

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