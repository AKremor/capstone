#include <src/reference_signal/sine_wave.h>
#include "arm_math.h"

ab0_coord SineWave::getValue(float32_t time) {
    float32_t magnitude = 10;     // TODO(akremor): Extract
    float32_t frequency_hz = 50;  // TODO(akremor): Extract

    // TODO(akremor): What will happen when the time value overflows?

    // TODO(akremor): Why am I defeated by arm_sin_f32
    float32_t a = magnitude * sinf(2 * PI * frequency_hz * time);
    float32_t b = magnitude * sinf(2 * PI * frequency_hz * time - 2 * PI / 3);

    // Pack into struct
    ab0_coord value = {0};
    arm_clarke_f32(a, b, &value.alpha, &value.beta);

    // Then abc-ab0
    // TODO(akremor): Generate straight up in ab0?

    return value;
}

abc_quantity SineWave::getValueAbc(float32_t time) {
    float32_t magnitude = 4;      // TODO(akremor): Extract
    float32_t frequency_hz = 20;  // TODO(akremor): Extract

    // TODO(akremor): What will happen when the time value overflows?

    // TODO(akremor): Why am I defeated by arm_sin_f32
    float32_t a = magnitude * sinf(2 * PI * frequency_hz * time);
    float32_t b = magnitude * sinf(2 * PI * frequency_hz * time - 2 * PI / 3);
    float32_t c = magnitude * sinf(2 * PI * frequency_hz * time + 2 * PI / 3);

    // Pack into struct
    abc_quantity value = {a, b, c};

    return value;
}
