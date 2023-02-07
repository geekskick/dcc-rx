#pragma once
#include <array>
#include <cstdint>
#include <string_view>
enum BufferState
{
    Empty,
    Full,
    Filling
};

std::ostream& operator<<(std::ostream& os, const BufferState& state){
    constexpr auto strings = std::array<std::string_view, 3>{"Empty", "Full", "Filling"};
    using T = std::underlying_type_t<std::__remove_cvref_t<decltype(state)>>;
    const auto idx = static_cast<T>(state);
    os << strings.at(idx);
    return os;
}

template<uint8_t NBytes>
class BitBuffer{
    std::array<uint8_t, NBytes> data_{};
    uint8_t current_bit_{};
    uint8_t current_byte_{};

    void wrap_increment_bit(){
        current_bit_ = (current_bit_ + 1) % 8;
        current_byte_ = current_bit_ == 0? current_byte_ + 1 : current_byte_;
    }
public:

    const uint8_t& at(const uint8_t idx) const {
        return data_.at(idx);
    }

    BufferState state() const {
        if(current_bit_ == 0 && current_byte_ == 0){
            return BufferState::Empty;
        }
        else if(current_byte_ == NBytes){
            return BufferState::Full;
        }else{
            return BufferState::Filling;
        }
    }

    BufferState push(const uint8_t bit){
        if(state() == BufferState::Full){
            return BufferState::Full;
        }
        const auto shift = 7 - current_bit_;
        const auto masked_bit = bit & 0x01;
        const auto shifted_bit = masked_bit << shift;
        data_.at(current_byte_) |= shifted_bit;
        wrap_increment_bit();
        return state(); 
    }
};