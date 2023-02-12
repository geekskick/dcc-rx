#include "bits/bit_buffer.hpp"
#include "bits/bit_factory.hpp"
#include "bits/bit_tolerance.hpp"
#include "gpio/gpio_decorator.hpp"
#include "gpio/pico_gpio.hpp"
#include "gpio_state/gpio_state.hpp"
#include "gpio_state/gpio_state_decorator.hpp"
#include "pico/stdio.h" // stdio_init_all
#include "states/collecting_data.hpp"
#include "states/state_machine.hpp"
#include "states/waiting_for_preamble.hpp"
#include "timestamp/pico_timestamp.hpp"
#include "timestamp/timestamp_decorator.hpp"
#include <iostream>

int main()
{
    constexpr GpioNumber LED_PIN = 25;
    constexpr GpioNumber GPIO_READ = 16; // Bottom right pin(21)

    stdio_init_all();

    auto raw_led = OutputGpio{LED_PIN, GpioStateInterface::Level::High};
    auto led = TogglingOutputGpioDecorator{raw_led};
    auto pin = PullDownInputGpio{GPIO_READ};
    auto pin_state_raw = GpioState{};
    auto pin_state = EdgeDetectingGpioDecorator{pin_state_raw};
    auto ts_raw = Timestamp{};
    auto ts = PulseWidthTimestampDecorator{ts_raw};

    // A 1 bit is two halves. The first half tols are
    // 55us - 61us
    // And the second half is
    // 53us - 64us
    // We are simplifying things massively and using the time between edges of the same type
    // so we combine them
    const auto one_tolerances = BitTolerance{Microseconds{108}, Microseconds{125}};

    // A 0 bit is two halves. The first half tols are
    // 95us - 9900us
    // And the second half is
    // 90us - 10000
    // We are simplifying things massively and using the time between edges of the same type
    // so we combine them
    const auto zero_tolerances = BitTolerance{Microseconds{185}, Microseconds{19900}};
    const auto bit_factory = BitFactory{zero_tolerances, one_tolerances};

    constexpr auto bits_in_packet = 33;
    auto buffer = BitBuffer{};

    auto preamble_state = WaitingForPreambleState{bit_factory};
    auto collecting_state = CollectingDataState{bit_factory, [int i = 0](BitBuffer::BufferType &buffer) mutable
                                                {
                                                    std::cout << i << "\t"
                                                              << "Packet: ";
                                                    for (const auto &b : buffer)
                                                    {
                                                        std::cout << std::hex << std::setfill('0') << std::setw(2) << b << " ";
                                                    }
                                                    std::cout << "\n";
                                                }};
    auto state_machine = StateMachine{preamble_state, collecting_state, buffer};

    while (1)
    {
        pin_state.update(pin.get());
        if (pin_state.detect_edge() == EdgeDetectingGpioDecorator::Edge::Rising)
        {
            ts.update();
            led.toggle();
            const auto pw = ts.pulse_width();
            state_machine.step(pw);
        }
        sleep_ms(500);
    }
}