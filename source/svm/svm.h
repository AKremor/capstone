#include "arm_math.h"

void svm_find_absolute_levels(int32_t g, int32_t h,
                              PhaseVoltageLevel* absolute_node);

void svm_modulator(float32_t d, float32_t q, float32_t sin_value,
                   float32_t cos_value, PhaseVoltageLevel nearest_nodes[3],
                   float32_t duty_cycle[3]);
