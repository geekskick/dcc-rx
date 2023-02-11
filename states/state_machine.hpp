#pragma once
#include "state_machine_interface.hpp"
#include <variant>

template<typename... States>
class StateMachine : public StateMachineInterface
{
    static_assert((std::is_base_of_v<StateInterface, States> && ...), "Invalid template types.");
    std::variant<States> current_state_{};
public:

    StateMachine(StateInterface& initial_state):
        current_state_{initial_state}{}


    void run(const Microseconds &pulse_width) override
    {
        // Implicit dependency here on the variant to be a StateInterface
        current_state_.run(pulse_width);
    }

    template<typename T>
    std::enable_if_t<std::is_base_of_v<StateInterface, T>> 
    transition_to(const T& next){
        current_state = next;
    }

    void reset_to_waiting_preamble() override
    {
        std::cout << "Transitioning to waiting_for_preamble\n";
        current_state_ = waiting_for_preamble_;
    }

    void transition_to_collecting_data() override
    {
        std::cout << "Transitioning to collecting_data\n";
        current_state_ = collecting_data_;
    }

    void transition_to_packet_received() override
    {
        std::cout << "Transitioning to packet_received\n";
        current_state_ = packet_recieved_;
    }
};