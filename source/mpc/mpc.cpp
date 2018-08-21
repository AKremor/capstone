#include <math.h>
#include <source/mpc/mpc.h>
#include <source/system_config.h>
#include <limits>

PhaseVoltageLevel findOptimalSwitchingIndex(SystemState *system_state,
                                            LoadModel *load) {
    // Now find best option, minimisation problem
    PhaseVoltageLevel best_state = {0, 0, 0};
    float best_cost_found = std::numeric_limits<float>::max();
    // TODO collapse desired state into system state

    for (int a = 0; a < sizeof(svm_phase_levels_a); a++) {
        for (int b = 0; b < sizeof(svm_phase_levels_b); b++) {
            for (int c = 0; c < sizeof(svm_phase_levels_c); c++) {
                PhaseVoltageLevel level;
                level.a = a - sizeof(svm_phase_levels_a) / 2;
                level.b = b - sizeof(svm_phase_levels_b) / 2;
                level.c = c - sizeof(svm_phase_levels_c) / 2;

                three_phase_quantity prediction =
                    predictSystemState(system_state->load_line_current,
                                       system_state->load_voltage, level, load);

                float cost = 0;
                cost += abs(prediction.get_ab0().a -
                            system_state->reference.get_dq0(1).d);
                cost += abs(prediction.get_ab0().b -
                            system_state->reference.get_dq0(1).q);
                cost += abs(prediction.get_ab0().zero -
                            system_state->reference.get_dq0(1).zero);

                if (cost < best_cost_found) {
                    best_cost_found = cost;
                    best_state = {a, b, c};
                }
            }
        }
    }

    return best_state;
}

three_phase_quantity predictSystemState(three_phase_quantity currents,
                                        three_phase_quantity voltages,
                                        const PhaseVoltageLevel voltage_level,
                                        LoadModel *load) {
    ab0_quantity prediction;
    prediction.a =
        (load->L * currents.get_ab0().a + load->Ts * voltage_level.a) *
        load->model_reciprocal_denominator;
    prediction.b =
        (load->L * currents.get_ab0().b + load->Ts * voltage_level.b) *
        load->model_reciprocal_denominator;
    prediction.zero = 0;

    three_phase_quantity out;
    out.set_ab0(prediction);
    return out;
}
