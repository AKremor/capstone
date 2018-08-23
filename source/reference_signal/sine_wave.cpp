#include <source/quantities.h>
#include <source/reference_signal/sine_wave.h>
#include <source/system_config.h>
#include "arm_math.h"

abc_quantity SineWave::getValueAbc(uint64_t n) {
    float32_t a = magnitude * cosf(2.0 * PI * frequency_hz * n / svm_timer_hz);
    float32_t b = magnitude *
                  cosf(2.0 * PI * frequency_hz * n / svm_timer_hz - 2 * PI / 3);
    float32_t c = magnitude *
                  cosf(2.0 * PI * frequency_hz * n / svm_timer_hz + 2 * PI / 3);

    abc_quantity value = {a, b, c};
    return value;
}
