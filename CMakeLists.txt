cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(test
    main.cc
    gpio/gpio_interface.hpp
    gpio/gpio_decorator.hpp
    gpio/gpio_decorator.cc
    gpio/gpio.hpp
    gpio/gpio.cc
    gpio_state/gpio_state_interface.hpp
    gpio_state/gpio_state.hpp
    gpio_state/gpio_state.cc
    gpio_state/gpio_state_decorator.hpp
    gpio_state/gpio_state_decorator.cc
    timestamp_interface.hpp
)
set_target_properties(test PROPERTIES LANGUAGE CXX)

pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 1) 
pico_add_extra_outputs(test) 
target_link_libraries(test pico_stdlib)