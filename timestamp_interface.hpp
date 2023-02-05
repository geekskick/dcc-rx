#pragma once

#include "pico/time.h"
#include <algorithm>

using Microseconds = uint64_t;

struct TimestampInterface{
    virtual const Microseconds& get() const = 0;
    virtual void update(const Microseconds& value) = 0;
    virtual void update() = 0;

};

struct Timestamp final :TimestampInterface{
    Timestamp() = default;
    Timestamp(Microseconds us): stamp_{us}{}
    const Microseconds& get() const override{
        return stamp_;
    }
    void update(const Microseconds& value)override{
        stamp_ = value;
    }
    void update() override
    {
        stamp_ = to_us_since_boot(get_absolute_time());
    }

private:
    Microseconds stamp_{to_us_since_boot(get_absolute_time())};
};


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

struct PulseWidthTimestamp final: public TimestampDecorator{
    PulseWidthTimestamp(TimestampInterface& wrappee): TimestampDecorator{wrappee}{}
    Microseconds pulse_width() const {
        const auto me = get();
        const auto highest = std::max(me, previous_timestamp_);
        const auto lowest = std::min(me, previous_timestamp_);
        return highest - lowest;
    }
    void update() override{
        previous_timestamp_ = wrappee_.get();
        wrappee_.update();
    }

    void update(const Microseconds& stamp) override {
        previous_timestamp_ = stamp;
        wrappee_.update(stamp);
    }
private:
    Microseconds previous_timestamp_{0};
};