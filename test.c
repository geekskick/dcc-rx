#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

const uint LED_PIN = 25;
const uint GPIO_READ = 16; // Bottom right pin(21)

typedef int64_t u_seconds_diff_t;

bool edge_detected(const bool previous_state, const bool current_state)
{
    return previous_state != current_state;
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_init(GPIO_READ);
    gpio_set_pulls(GPIO_READ, false, true);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(GPIO_READ, GPIO_IN);
    bool previous_pin_state = false;
    absolute_time_t previous_rising_edge = get_absolute_time();
    printf("[booting] %d\n", to_ms_since_boot(previous_rising_edge));

    while (1)
    {
        const bool current_pin_state = gpio_get(GPIO_READ);
        const bool edge_detected = previous_pin_state != current_pin_state;
        if (edge_detected)
        {
            const absolute_time_t current_time = get_absolute_time();
            printf("EDGE @%lldus\n", to_us_since_boot(current_time));
            const bool rising_edge = current_pin_state == true;
            if (rising_edge)
            {
                if (is_nil_time(current_time))
                {
                    printf("current time is nil \n");
                }
                if (is_nil_time(previous_rising_edge))
                {
                    printf("previous time is nil \n");
                }

                const u_seconds_diff_t pulse_width = absolute_time_diff_us(previous_rising_edge, current_time);
                printf("Rising Edge Width us = %lldus\t(|%lld - %lld|)\n",
                       pulse_width,
                       to_us_since_boot(current_time),
                       to_us_since_boot(previous_rising_edge));

                previous_rising_edge = current_time;
            }
        }
        previous_pin_state = current_pin_state;
        sleep_ms(30);
    }
}