#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "states/waiting_for_preamble.hpp"
/*
The preamble to a packet consists of a sequence of "1" bits. 
A digital decoder must not accept as a valid, any preamble that 
has less then 10 complete one bits, or require for proper reception 
of a packet with more than 12 complete one bits. A command 
station must send a minimum of 14 full preamble bits.
*/

TEST(WaitingForPremableState, TransitionsOnCorrectPreamble){
    auto uut =  
}