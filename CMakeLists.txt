cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

#[[
TODO: Enable cross and native compilation based on the target
TODO: Turn on catch
]]
add_library(core INTERFACE
    gpio/gpio_interface.hpp
    gpio/gpio_decorator.hpp
    gpio/gpio.hpp
    gpio_state/gpio_state_interface.hpp
    gpio_state/gpio_state.hpp
    gpio_state/gpio_state_decorator.hpp
    timestamp/timestamp_interface.hpp
    timestamp/timestamp.hpp
    timestamp/timestamp_decorator.hpp
)

target_include_directories(core INTERFACE .)
target_link_libraries(core INTERFACE pico_stdlib)


add_executable(main
    main.cc
)
set_target_properties(main core PROPERTIES LANGUAGE CXX)
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1) 
pico_add_extra_outputs(main) 
target_link_libraries(main core)

enable_testing()