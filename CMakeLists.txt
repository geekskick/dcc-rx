cmake_minimum_required(VERSION 3.13)

if("${ARCH}" STREQUAL "pico")
    include(pico_sdk_import.cmake)
endif()

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_library(core INTERFACE
    gpio/gpio_interface.hpp
    gpio/gpio_decorator.hpp
    gpio_state/gpio_state_interface.hpp
    gpio_state/gpio_state.hpp
    gpio_state/gpio_state_decorator.hpp
    timestamp/timestamp_interface.hpp
    timestamp/timestamp_decorator.hpp
    timestamp/microseconds.hpp
    bits/bit_tolerance.hpp
    bits/bit_factory.hpp
    bits/buffer_interface.hpp
    bits/bit_buffer.hpp
    states/state_interface.hpp
    states/state_machine_interface.hpp
    states/state_machine.hpp
    states/waiting_for_preamble.hpp
    states/collecting_data.hpp
    states/template_state.hpp
)

set_target_properties(core PROPERTIES LANGUAGE CXX)
target_include_directories(core INTERFACE .)

if("${ARCH}" STREQUAL "pico")
    message(STATUS "Cross Compiling by default, so importing pico stuff")

    pico_sdk_init()
    add_library(hal INTERFACE 
        timestamp/pico_timestamp.hpp
        gpio/pico_gpio.hpp
    )
    target_link_libraries(hal INTERFACE pico_stdlib core)
    set_target_properties(hal PROPERTIES LANGUAGE CXX)
    target_include_directories(hal INTERFACE .)

    add_executable(main
        main.cc
    )
    set_target_properties(core PROPERTIES LANGUAGE CXX)
    pico_enable_stdio_usb(main 1)
    pico_enable_stdio_uart(main 1) 
    pico_add_extra_outputs(main) 
    target_link_libraries(main core hal)


elseif("${ARCH}" STREQUAL "native")
    message(STATUS "Compiling natively")

    add_link_options(-fuse-ld=mold)

    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "Using ccache")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
    )
    FetchContent_MakeAvailable(googletest)
    add_executable(
        test
        tests/timestamp_test.cc
        tests/state_machine_test.cc
        tests/gpio_test.cc
        tests/bit_test.cc
        tests/preamble_state_test.cc
        tests/collecting_state_test.cc
        tests/mock_state_machine.hpp
        tests/bitbuffer_test.cc
    )
    target_link_libraries(
        test
        GTest::gtest_main
        GTest::gmock
        core
    )

    include(GoogleTest)
    gtest_discover_tests(test)
else()
    message(FATAL_ERROR "Unrecognised Arch ${ARCH}")
endif()
