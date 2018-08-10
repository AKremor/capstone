#include <source/reference_signal/sine_wave.h>
#include <source/system_config.h>
#include "arm_math.h"

abc_quantity SineWave::getValueAbc(float32_t time) {
    // TODO(akremor): Pull out /1000
    float32_t a = magnitude * cosf(2 * PI * frequency_hz * time / 1000);
    float32_t b =
        magnitude * cosf(2 * PI * frequency_hz * time / 1000 - 2 * PI / 3);
    float32_t c =
        magnitude * cosf(2 * PI * frequency_hz * time / 1000 + 2 * PI / 3);

    abc_quantity value = {a, b, c};
    return value;
}
