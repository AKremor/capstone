#include <source/quantities.h>
#include <source/system_state.h>
#include "arm_math.h"

class LoadModel {
   public:
    float32_t R;
    float32_t L;
    float32_t C;
    float32_t Ts;
    float32_t model_reciprocal_denominator;
};

three_phase_quantity predictSystemState(three_phase_quantity currents,
                                        three_phase_quantity voltages,
                                        const PhaseVoltageLevel voltage_level,
                                        LoadModel *load);

int findOptimalSwitchingIndex(SystemState *system_state, LoadModel *load);

void setGateSignals(PhaseVoltageLevel level_selection);
