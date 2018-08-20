#pragma once
#include <source/quantities.h>
#include <stdint.h>

class SystemState {
   public:
    static SystemState* get() {
        static SystemState instance;
        return &instance;
    }
    int32_t a_phase;
    int32_t b_phase;
    int32_t c_phase;

    three_phase_quantity reference;
    three_phase_quantity load_voltage;
    three_phase_quantity load_line_current;
    three_phase_quantity load_ll_voltage;

   private:
    SystemState(){};
    SystemState(SystemState const&);
    void operator=(SystemState const&);
};
