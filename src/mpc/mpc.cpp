#include <math.h>
#include <src/mpc/mpc.h>
#include <src/mpc/switching_states.h>
#include <xdc/runtime/System.h>

//#include "arm_math.h"
//#include "arm_const_structs.h"

int findOptimalSwitchingIndex(SystemState system_state,
                              DesiredState desired_state,
                              const int cell_states[n_levels][n_phases]) {
  DesiredState predictions[n_levels] = {NULL};

  // Now find best option, minimisation problem
  int best_index_found = 0;  // TODO
  float best_cost_found = 100000000;  // TODO
  // TODO collapse desired state into system state
  for (int i = 0; i < n_levels; i++) {
    // TODO Add the model in
    predictions[i].desired_current_alpha = 0.5 * cell_states[i][0];
    predictions[i].desired_current_beta = 0.5 * cell_states[i][1];
    predictions[i].desired_current_zero = 0;

    float cost = abs(predictions[i].desired_current_alpha -
                   desired_state.desired_current_alpha);
    cost += abs(predictions[i].desired_current_beta -
                    desired_state.desired_current_beta);
    cost += abs(predictions[i].desired_current_zero -
                    desired_state.desired_current_zero);

    if (cost < best_cost_found) {
      best_cost_found = cost;
      best_index_found = i;
    }
  }

  return best_index_found;
}
