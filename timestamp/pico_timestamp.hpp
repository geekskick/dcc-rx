#pragma once
#include "timestamp_interface.hpp"

#include "pico/time.h"
struct Timestamp final : TimestampInterface
{
    Timestamp() = default;
    Timestamp(Microseconds us) : stamp_{us} {}
    const Microseconds &get() const override
    {
        return stamp_;
    }
    void update(const Microseconds &value) override
    {
        stamp_ = value;
    }
    void update() override
    {
        stamp_ = to_us_since_boot(get_absolute_time());
    }

private:
    Microseconds stamp_{to_us_since_boot(get_absolute_time())};
};