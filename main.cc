#include "gpio/gpio.hpp"
#include "gpio/gpio_decorator.hpp"
#include "gpio_state/gpio_state.hpp"
#include "gpio_state/gpio_state_decorator.hpp"
#include "pico/stdio.h" // stdio_init_all
#include "timestamp_interface.hpp"
#include <iostream>

const GpioNumber LED_PIN = 25;
const GpioNumber GPIO_READ = 16; // Bottom right pin(21)

typedef int64_t u_seconds_diff_t;

int main()
{
    stdio_init_all();

    auto raw_led = OutputGpio{LED_PIN, GpioStateInterface::Level::High};
    auto led = TogglingOutputGpioDecorator{raw_led};
    auto pin = PullDownInputGpio{GPIO_READ};
    auto pin_state_raw = GpioState{};
    auto pin_state = EdgeDetectingGpioDecorator{pin_state_raw};
    auto ts_raw = Timestamp{};
    auto ts = PulseWidthTimestampDecorator{ts_raw};

    while (1)
    {
        pin_state.update(pin.get());
        if (pin_state.detect_edge() == EdgeDetectingGpioDecorator::Edge::Rising)
        {
            ts.update();
            led.toggle();
            std::cout << "Pulse Width = " << ts.pulse_width() << "us\n";
        }
        sleep_ms(500);
    }
}