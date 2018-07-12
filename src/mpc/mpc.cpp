#include <math.h>
#include <src/mpc/mpc.h>
#include <src/mpc/switching_states.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <xdc/runtime/System.h>
#include <limits>
//#include "arm_const_structs.h"

int findOptimalSwitchingIndex(SystemState *system_state,
                              SystemState *desired_state,
                              const PhaseVoltageLevel cell_states[n_levels],
                              LoadModel *load) {
    // Now find best option, minimisation problem
    int best_index_found = 0;
    float best_cost_found = std::numeric_limits<float>::max();
    // TODO collapse desired state into system state
    for (int i = 0; i < n_levels; i++) {
        SystemState prediction =
            predictSystemState(system_state, cell_states[i], load);

        float cost = 0;
        cost += abs(prediction.current_alpha - desired_state->current_alpha);
        cost += abs(prediction.current_beta - desired_state->current_beta);
        cost += abs(prediction.current_zero - desired_state->current_zero);

        if (cost < best_cost_found) {
            best_cost_found = cost;
            best_index_found = i;
        }
    }

    return best_index_found;
}

void setGateSignals(PhaseVoltageLevel level_selection) {
    uint8_t gpio_K = gate_signals[level_selection.a];
    uint8_t gpio_L = gate_signals[level_selection.b];
    uint8_t gpio_M = gate_signals[level_selection.c];

    GPIOPinWrite(GPIO_PORTK_BASE, gpio_K, gpio_K);
    GPIOPinWrite(GPIO_PORTL_BASE, gpio_L, gpio_L);
    GPIOPinWrite(GPIO_PORTM_BASE, gpio_M, gpio_M);
}

SystemState predictSystemState(SystemState *current_state,
                               const PhaseVoltageLevel voltage_level,
                               LoadModel *load) {
    // TODO alpha beta transform on voltages
    SystemState prediction;
    // TODO replace the 1 with v_alpha
    prediction.current_alpha =
        (load->L * current_state->current_alpha + load->Ts * 1) *
        load->model_reciprocal_denominator;
    prediction.current_beta =
        (load->L * current_state->current_beta + load->Ts * 1) *
        load->model_reciprocal_denominator;

    prediction.current_zero = 0;

    return prediction;
}
