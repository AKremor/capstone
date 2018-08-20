#include <source/quantities.h>
#include <source/svm/svm.h>
#include <source/system_config.h>
#include "arm_math.h"

abc_quantity svm_modulator(float32_t d, float32_t q, float32_t sin_value,
                   float32_t cos_value) {
    float32_t Ialphacontrol, Ibetacontrol;

    arm_inv_park_f32(d, q, &Ialphacontrol, &Ibetacontrol, sin_value, cos_value);

    float32_t Ia, Ib, Ic;
    arm_inv_clarke_f32(Ialphacontrol, Ibetacontrol, &Ia, &Ib);
    Ic = -1.0 * (Ia + Ib);

    gh_quantity hex_value;
    hex_value.g = 1 / (3 * Vdc) * (2 * Ia - Ib - Ic);
    hex_value.h = 1 / (3 * Vdc) * (-1 * Ia + 2 * Ib - Ic);

    // Ordered ul, lu, uu, ll
    gh_quantity nodes[4] = {NULL};

    nodes[0] = {ceilf(hex_value.g), floorf(hex_value.h)};
    nodes[1] = {floorf(hex_value.g), ceilf(hex_value.h)};
    nodes[2] = {ceilf(hex_value.g), ceilf(hex_value.h)};
    nodes[3] = {floorf(hex_value.g), floorf(hex_value.h)};

    // Ordered ul, lu, uu, ll
    float32_t distances[4] = {NULL};

    // Calculate nearest node
    distances[0] =
        fabs(nodes[0].g - hex_value.g) + fabs(nodes[0].h - hex_value.h);
    distances[1] =
        fabs(nodes[1].g - hex_value.g) + fabs(nodes[1].h - hex_value.h);
    distances[2] =
        fabs(nodes[2].g - hex_value.g) + fabs(nodes[2].h - hex_value.h);
    distances[3] =
        fabs(nodes[3].g - hex_value.g) + fabs(nodes[3].h - hex_value.h);

    float32_t min_distance = 1000000;
    int min_index = 0;
    for (int i = 0; i < 4; i++) {
        if (distances[i] < min_distance) {
            min_distance = distances[i];
            min_index = i;
        }
    }

    gh_quantity nearest_1 = nodes[min_index];

    // Now we need to find an available voltage state.
    // This is a very rudimentary implementation

    int32_t k = 0;
    bool constraints_satisfied = false;
    int32_t g = nearest_1.g;
    int32_t h = nearest_1.h;
    int32_t n = n_levels;
    while (!constraints_satisfied) {
        if (k >= 0 && k - g >= 0 && k - g - h >= 0 && k <= n - 1 &&
            k - g <= n - 1 && k - g - h <= n - 1) {
            constraints_satisfied = true;
            break;
        }
        if (k >= n - 1) {
            // Saturation/error in modulator
            break;
        }
        k++;
    }

    int32_t a_phase = k;
    int32_t b_phase = k - nearest_1.g;
    int32_t c_phase = k - nearest_1.g - nearest_1.h;

    // Ensure no levels are saturating
    if (a_phase >= n_levels) {
        a_phase = n_levels - 1;
    }

    if (a_phase < 0) {
        a_phase = 0;
    }

    if (b_phase >= n_levels) {
        b_phase = n_levels - 1;
    }

    if (b_phase < 0) {
        b_phase = 0;
    }

    if (c_phase >= n_levels) {
        c_phase = n_levels - 1;
    }

    if (c_phase < 0) {
        c_phase = 0;
    }

    return {a_phase, b_phase, c_phase};
}
