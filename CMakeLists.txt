cmake_minimum_required(VERSION 3.13)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()
option(TEST "Enable Testing" OFF)

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

set_target_properties(core PROPERTIES LANGUAGE CXX)
target_include_directories(core INTERFACE .)
target_link_libraries(core INTERFACE pico_stdlib)

if(NOT ${TEST})
    message(STATUS "Cross Compiling by default, so importing pico stuff")
    include(pico_sdk_import.cmake)
    pico_sdk_init()

    add_executable(main
        main.cc
    )
    set_target_properties(core PROPERTIES LANGUAGE CXX)
    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 1) 
    pico_add_extra_outputs(main) 
    target_link_libraries(main core)
else()
    message(STATUS "Compiling natively")
    enable_testing()
    include(FetchContent)

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
    )
    FetchContent_MakeAvailable(googletest)
    add_executable(
        hello_test
        hello_test.cc
    )
    target_link_libraries(
        hello_test
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(hello_test)
endif()
