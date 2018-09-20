#include <source/quantities.h>
#include <source/svm/svm.h>
#include <source/system_config.h>
#include "arm_math.h"
#include "assert.h"

constexpr int32_t lookup_table_offset = 8;
static int32_t k_lookup_table[17][17] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {-1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},
    {-1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3},
    {-1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4},
    {-1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5},
    {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6},
    {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, -1},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8, -1, -1},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8, -1, -1, -1},
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, -1, -1, -1, -1},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, -1, -1, -1, -1, -1},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, -1, -1, -1, -1, -1, -1},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {8, 8, 8, 8, 8, 8, 8, 8, 8, -1, -1, -1, -1, -1, -1, -1, -1},
};

void svm_find_absolute_levels(int32_t g, int32_t h,
                              PhaseVoltageLevel* absolute_node) {
    int32_t k =
        k_lookup_table[g + lookup_table_offset][h + lookup_table_offset];
    int32_t a_phase = k;
    int32_t b_phase = k - g;
    int32_t c_phase = k - g - h;

    // Array indices, not actual voltages
    absolute_node->a = a_phase;
    absolute_node->b = b_phase;
    absolute_node->c = c_phase;
}

void svm_modulator(float32_t d, float32_t q, float32_t sin_value,
                   float32_t cos_value, PhaseVoltageLevel nearest_nodes[3],
                   float32_t duty_cycle[3]) {
    float32_t Ialphacontrol, Ibetacontrol;

    arm_inv_park_f32(d, q, &Ialphacontrol, &Ibetacontrol, sin_value, cos_value);

    float32_t Ia, Ib, Ic;
    arm_inv_clarke_f32(Ialphacontrol, Ibetacontrol, &Ia, &Ib);
    Ic = -1.0 * (Ia + Ib);

    gh_quantity hex_value;
    hex_value.g = 1 / (3 * Vdc) * (2 * Ia - Ib - Ic);
    hex_value.h = 1 / (3 * Vdc) * (-1 * Ia + 2 * Ib - Ic);

    // Ordered ul, lu, uu, ll
    gh_quantity nodes[4];

    nodes[0] = {ceilf(hex_value.g), floorf(hex_value.h)};
    nodes[1] = {floorf(hex_value.g), ceilf(hex_value.h)};
    nodes[2] = {ceilf(hex_value.g), ceilf(hex_value.h)};
    nodes[3] = {floorf(hex_value.g), floorf(hex_value.h)};

    // Vul
    svm_find_absolute_levels(nodes[0].g, nodes[0].h, nearest_nodes + 0);
    // Vlu
    svm_find_absolute_levels(nodes[1].g, nodes[1].h, nearest_nodes + 1);

    if (hex_value.g + hex_value.h - nodes[0].g - nodes[0].h > 0) {
        // Vuu
        svm_find_absolute_levels(nodes[2].g, nodes[2].h, nearest_nodes + 2);
        duty_cycle[0] = -1 * (hex_value.h - nodes[2].h);
        duty_cycle[1] = -1 * (hex_value.g - nodes[2].g);
        duty_cycle[2] = 1 - duty_cycle[0] - duty_cycle[1];
    } else {
        // Vll
        svm_find_absolute_levels(nodes[3].g, nodes[3].h, nearest_nodes + 2);
        duty_cycle[0] = hex_value.g - nodes[3].g;
        duty_cycle[1] = hex_value.h - nodes[3].h;
        duty_cycle[2] = 1 - duty_cycle[0] - duty_cycle[1];
    }
}
