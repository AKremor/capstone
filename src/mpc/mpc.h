#include <src/mpc/switching_states.h>
#include "arm_math.h"

class SystemState {
   public:
    float32_t current_alpha;
    float32_t current_beta;
    float32_t current_zero;
};

class LoadModel {
   public:
    float32_t R;
    float32_t L;
    float32_t C;
    float32_t Ts;
    float32_t model_reciprocal_denominator;
};

SystemState predictSystemState(SystemState *current_state,
                               const PhaseVoltageLevel voltage_level,
                               LoadModel *load);

int findOptimalSwitchingIndex(SystemState *system_state,
                              SystemState *desired_state,
                              const PhaseVoltageLevel cell_states[n_levels],
                              LoadModel *load);

void setGateSignals(PhaseVoltageLevel level_selection);
