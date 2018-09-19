#include <source/quantities.h>
#include <source/reference_signal/sine_wave.h>
#include <source/system_config.h>
#include "arm_math.h"

abc_quantity SineWave::getValueAbc(uint64_t microseconds) {
    float32_t theta = 2.0 * PI * frequency_hz * microseconds * 1E-6;
    float32_t a = magnitude * arm_cos_f32(theta);
    float32_t b = magnitude * arm_cos_f32(theta - 2 * PI / 3);
    float32_t c = magnitude * arm_cos_f32(theta + 2 * PI / 3);

    abc_quantity value = {a, b, c};
    return value;
}
