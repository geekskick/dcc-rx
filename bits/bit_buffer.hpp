#pragma once
#include <array>
#include <cstdint>
template<uint8_t NBytes>
struct BitBuffer{
    std::array<uint8_t, NBytes> data;
};