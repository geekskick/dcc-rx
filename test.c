#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdio.h>

const uint LED_PIN = 25;
const uint GPIO_READ = 16; // Bottom right pin(21)
int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_init(GPIO_READ);
    gpio_set_pulls(GPIO_READ, false, true);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(GPIO_READ, GPIO_IN);
    while (1)
    {
        const bool pin_state = gpio_get(GPIO_READ);
        gpio_put(LED_PIN, pin_state);
        puts("Hello World\n");
        sleep_ms(1000);
    }
}