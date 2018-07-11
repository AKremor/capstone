#include <src/mpc/switching_states.h>

class SystemState {
   public:
    float current_alpha;
    float current_beta;
    float current_zero;
};

class LoadModel {
   public:
    float R;
    float L;
    float C;
    float Ts;
};

SystemState predictSystemState(SystemState current_state,
                               PhaseVoltageLevel voltage_level, LoadModel load);

int findOptimalSwitchingIndex(SystemState system_state,
                              SystemState desired_state,
                              const PhaseVoltageLevel cell_states[n_levels],
                              LoadModel load);

void setGateSignals(PhaseVoltageLevel level_selection);
