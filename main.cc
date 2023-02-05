#include "gpio/gpio.hpp"
#include "gpio/gpio_decorator.hpp"
#include "gpio_state/gpio_state.hpp"
#include "gpio_state/gpio_state_decorator.hpp"
#include "pico/stdio.h" // stdio_init_all
#include "timestamp_interface.hpp"
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

    while (1)
    {
        pin_state.update(pin.get());
        if (pin_state.detect_edge() == EdgeDetectingGpioDecorator::Edge::Rising)
        {
            ts.update();
            led.toggle();
            std::cout << "Pulse Width = " << ts.pulse_width() << "us\n";
            // TODO: Take this pulse width and make it a 0, or a 1 and put into into some buffer
            // If the number of bits in the DCC protocol is low (<64) I can just stick it in a longlong
            // which might be nicer? In reality I think I'll have a class which does 
            // DCCMessage::append_bit(DCCMessage::Bit) -> void;
            // DCCMessage::reset() -> void;
            // DCCMessage::is_invalid() -> bool;
            // DCCMessage::is_complete() -> bool;
            // DCCMessage::address() -> uint8_t
            // DCCMessage::data() -> uint64_t;
            // 
            // then I can just chuck this out over serial and see packets on the screen
            // maybe add an id to each packet (effectively a SEQ number to show the counter on the screen)
        }
        sleep_ms(500);
    }
}