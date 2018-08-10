#include <src/reference_signal/sine_wave.h>
#include "arm_math.h"

abc_quantity SineWave::getValueAbc(float32_t time) {
    int n = 3;
    int Vdc = 1;
    float32_t magnitude = 0.866 * 1.141 * (n - 1) * Vdc;
    float32_t frequency_hz = 20;  // TODO(akremor): Extract

    float32_t a = magnitude * cosf(2 * PI * frequency_hz * time / 1000);
    float32_t b =
        magnitude * cosf(2 * PI * frequency_hz * time / 1000 - 2 * PI / 3);
    float32_t c =
        magnitude * cosf(2 * PI * frequency_hz * time / 1000 + 2 * PI / 3);

    abc_quantity value = {a, b, c};
    return value;
}
