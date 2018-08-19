#pragma once

#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

constexpr bool use_hil = true;

constexpr float32_t Kp = 100;
constexpr float32_t Ki = 10;
constexpr float32_t Kd = 0;

constexpr uint32_t system_clock_mhz = 120;

static const uint32_t chopper_hz = 10000;
static const uint32_t chopper_rising_dead_band_ns = 3600;
static const uint32_t chopper_falling_dead_band_ns = 3600;

// 12000, 1000, 1000 for 10kHz
// 6000, 672, 672 for 20kHz
// 3000, 1000, 1000 for 40kHz

// Internal reference generation
static constexpr uint16_t svm_timer_hz = 500;
static constexpr int32_t n_levels = 27;
static constexpr float32_t Vdc = 1;
static constexpr float32_t magnitude = 0.02;
static constexpr float32_t frequency_hz = 20;

enum hb_pin {
    A_POS1 = 0x01,  // PL0
    A_OFF1 = 0x00,  // N/A
    A_NEG1 = 0x02,  // PL1
    A_POS3 = 0x04,  // PL2
    A_OFF3 = 0x00,  // N/A
    A_NEG3 = 0x08,  // PL3
    A_POS9 = 0x10,  // PL4
    A_OFF9 = 0x00,  // N/A
    A_NEG9 = 0x20,  // PL5

    B_POS1 = 0x01,  // PK0
    B_OFF1 = 0x00,  // N/A
    B_NEG1 = 0x02,  // PK1
    B_POS3 = 0x04,  // PK2
    B_OFF3 = 0x00,  // N/A
    B_NEG3 = 0x08,  // PK3
    B_POS9 = 0x10,  // PK4
    B_OFF9 = 0x00,  // N/A
    B_NEG9 = 0x20,  // PK5

    C_POS1 = 0x01,  // PA0
    C_OFF1 = 0x00,  // N/A
    C_NEG1 = 0x02,  // PA1
    C_POS3 = 0x04,  // PA2
    C_OFF3 = 0x00,  // N/A
    C_NEG3 = 0x08,  // PA3
    C_POS9 = 0x10,  // PA4
    C_OFF9 = 0x00,  // N/A
    C_NEG9 = 0x20,  // PA5

};

constexpr uint8_t svm_phase_levels_a[] = {
    A_NEG9 | A_NEG3 | A_NEG1, A_NEG9 | A_NEG3 | A_OFF1,
    A_NEG9 | A_NEG3 | A_POS1, A_NEG9 | A_OFF3 | A_NEG1,
    A_NEG9 | A_OFF3 | A_OFF1, A_NEG9 | A_OFF3 | A_POS1,
    A_NEG9 | A_POS3 | A_NEG1, A_NEG9 | A_POS3 | A_OFF1,
    A_NEG9 | A_POS3 | A_POS1, A_OFF9 | A_NEG3 | A_NEG1,
    A_OFF9 | A_NEG3 | A_OFF1, A_OFF9 | A_NEG3 | A_POS1,
    A_OFF9 | A_OFF3 | A_NEG1, A_OFF9 | A_OFF3 | A_OFF1,
    A_OFF9 | A_OFF3 | A_POS1, A_OFF9 | A_POS3 | A_NEG1,
    A_OFF9 | A_POS3 | A_OFF1, A_OFF9 | A_POS3 | A_POS1,
    A_POS9 | A_NEG3 | A_NEG1, A_POS9 | A_NEG3 | A_OFF1,
    A_POS9 | A_NEG3 | A_POS1, A_POS9 | A_OFF3 | A_NEG1,
    A_POS9 | A_OFF3 | A_OFF1, A_POS9 | A_OFF3 | A_POS1,
    A_POS9 | A_POS3 | A_NEG1, A_POS9 | A_POS3 | A_OFF1,
    A_POS9 | A_POS3 | A_POS1};

constexpr uint8_t svm_phase_levels_b[] = {
    B_NEG9 | B_NEG3 | B_NEG1, B_NEG9 | B_NEG3 | B_OFF1,
    B_NEG9 | B_NEG3 | B_POS1, B_NEG9 | B_OFF3 | B_NEG1,
    B_NEG9 | B_OFF3 | B_OFF1, B_NEG9 | B_OFF3 | B_POS1,
    B_NEG9 | B_POS3 | B_NEG1, B_NEG9 | B_POS3 | B_OFF1,
    B_NEG9 | B_POS3 | B_POS1, B_OFF9 | B_NEG3 | B_NEG1,
    B_OFF9 | B_NEG3 | B_OFF1, B_OFF9 | B_NEG3 | B_POS1,
    B_OFF9 | B_OFF3 | B_NEG1, B_OFF9 | B_OFF3 | B_OFF1,
    B_OFF9 | B_OFF3 | B_POS1, B_OFF9 | B_POS3 | B_NEG1,
    B_OFF9 | B_POS3 | B_OFF1, B_OFF9 | B_POS3 | B_POS1,
    B_POS9 | B_NEG3 | B_NEG1, B_POS9 | B_NEG3 | B_OFF1,
    B_POS9 | B_NEG3 | B_POS1, B_POS9 | B_OFF3 | B_NEG1,
    B_POS9 | B_OFF3 | B_OFF1, B_POS9 | B_OFF3 | B_POS1,
    B_POS9 | B_POS3 | B_NEG1, B_POS9 | B_POS3 | B_OFF1,
    B_POS9 | B_POS3 | B_POS1};

constexpr uint8_t svm_phase_levels_c[] = {
    C_NEG9 | C_NEG3 | C_NEG1, C_NEG9 | C_NEG3 | C_OFF1,
    C_NEG9 | C_NEG3 | C_POS1, C_NEG9 | C_OFF3 | C_NEG1,
    C_NEG9 | C_OFF3 | C_OFF1, C_NEG9 | C_OFF3 | C_POS1,
    C_NEG9 | C_POS3 | C_NEG1, C_NEG9 | C_POS3 | C_OFF1,
    C_NEG9 | C_POS3 | C_POS1, C_OFF9 | C_NEG3 | C_NEG1,
    C_OFF9 | C_NEG3 | C_OFF1, C_OFF9 | C_NEG3 | C_POS1,
    C_OFF9 | C_OFF3 | C_NEG1, C_OFF9 | C_OFF3 | C_OFF1,
    C_OFF9 | C_OFF3 | C_POS1, C_OFF9 | C_POS3 | C_NEG1,
    C_OFF9 | C_POS3 | C_OFF1, C_OFF9 | C_POS3 | C_POS1,
    C_POS9 | C_NEG3 | C_NEG1, C_POS9 | C_NEG3 | C_OFF1,
    C_POS9 | C_NEG3 | C_POS1, C_POS9 | C_OFF3 | C_NEG1,
    C_POS9 | C_OFF3 | C_OFF1, C_POS9 | C_OFF3 | C_POS1,
    C_POS9 | C_POS3 | C_NEG1, C_POS9 | C_POS3 | C_OFF1,
    C_POS9 | C_POS3 | C_POS1};
