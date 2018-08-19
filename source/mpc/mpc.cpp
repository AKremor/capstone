#include <math.h>
#include <source/mpc/mpc.h>
#include <source/system_config.h>
#include <limits>

int findOptimalSwitchingIndex(SystemState *system_state,
                              const PhaseVoltageLevel cell_states[n_levels],
                              LoadModel *load) {
    // Now find best option, minimisation problem
    int best_index_found = 0;
    float best_cost_found = std::numeric_limits<float>::max();
    // TODO collapse desired state into system state
    for (int i = 0; i < n_levels; i++) {
        three_phase_quantity prediction = predictSystemState(
            system_state->load_line_current, system_state->load_voltage,
            cell_states[i], load);

        float cost = 0;
        // TODO Values in here
        cost +=
            abs(prediction.get_ab0().a - system_state->reference.get_dq0(1).d);
        cost +=
            abs(prediction.get_ab0().b - system_state->reference.get_dq0(1).q);
        cost += abs(prediction.get_ab0().zero -
                    system_state->reference.get_dq0(1).zero);

        if (cost < best_cost_found) {
            best_cost_found = cost;
            best_index_found = i;
        }
    }

    return best_index_found;
}

three_phase_quantity predictSystemState(three_phase_quantity currents,
                                        three_phase_quantity voltages,
                                        const PhaseVoltageLevel voltage_level,
                                        LoadModel *load) {
    ab0_quantity prediction;
    prediction.a =
        (load->L * currents.get_ab0().a + load->Ts * voltages.get_ab0().a) *
        load->model_reciprocal_denominator;
    prediction.b =
        (load->L * currents.get_ab0().b + load->Ts * voltages.get_ab0().b) *
        load->model_reciprocal_denominator;
    prediction.zero = 0;

    three_phase_quantity out;
    out.set_ab0(prediction);
    return out;
}
