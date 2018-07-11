#include <src/mpc/switching_states.h>

class SystemState {
 public:
  float current_alpha;
  float current_beta;
  float current_zero;
};

class DesiredState {
 public:
  float desired_current_alpha;
  float desired_current_beta;
  float desired_current_zero;
};

 int findOptimalSwitchingIndex(SystemState system_state,
                              DesiredState desired_state,
                              const int cell_states[n_levels][n_phases]);
