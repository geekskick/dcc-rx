#pragma once
#include "timestamp_interface.hpp"
struct TimestampDecorator : public TimestampInterface
{
    virtual const Microseconds &get() const override
    {
        return wrappee_.get();
    }
    virtual void update(const Microseconds &value) override
    {
        wrappee_.update(value);
    }
    virtual void update() override
    {
        wrappee_.update();
    }

protected:
    TimestampInterface &wrappee_;
    TimestampDecorator(TimestampInterface &wrappee) : wrappee_{wrappee} {}
};

struct PulseWidthTimestampDecorator final : public TimestampDecorator
{
    PulseWidthTimestampDecorator(TimestampInterface &wrappee) : TimestampDecorator{wrappee} {}
    Microseconds pulse_width() const
    {
        const auto me = get();
        const auto highest = std::max(me, previous_timestamp_);
        const auto lowest = std::min(me, previous_timestamp_);
        return highest - lowest;
    }
    void update() override
    {
        previous_timestamp_ = wrappee_.get();
        wrappee_.update();
    }

    void update(const Microseconds &stamp) override
    {
        previous_timestamp_ = stamp;
        wrappee_.update(stamp);
    }

private:
    Microseconds previous_timestamp_{0};
};