#pragma once

#include <stdint.h>
#include "arm_math.h"

constexpr bool use_closed_loop = false;

constexpr float32_t Kp = 1;
constexpr float32_t Ki = 12;
constexpr float32_t Kd = 0;

constexpr uint32_t system_clock_mhz = 120;

// Chopper configuration
// 12000, 1000, 1000 for 10kHz
// 6000, 672, 672 for 20kHz
// 3000, 1000, 1000 for 40kHz
static const uint32_t chopper_hz = 10000;
static const uint32_t chopper_rising_dead_band_ns = 3600;
static const uint32_t chopper_falling_dead_band_ns = 3600;

// Internal reference generation
static constexpr uint16_t svm_period_us = 200;
static constexpr uint16_t adc_period_us = 50;  // Not used
static constexpr int16_t pwm_period_div = 1;
static constexpr int16_t pwm_period_us = svm_period_us / pwm_period_div;
static constexpr int32_t n_levels = 27;
static constexpr float32_t Vdc = 1;
static constexpr float32_t magnitude = 13;  // This is peak
static constexpr float32_t fundamental_frequency_hz = 100;

enum hb_pin {
    A_POS1 = 0x01,  // PL0
    A_OFF1 = 0x00,  // N/A
    A_NEG1 = 0x02,  // PL1
    B_POS1 = 0x04,  // PL2
    B_OFF1 = 0x00,  // N/A
    B_NEG1 = 0x08,  // PL3
    C_POS1 = 0x10,  // PL4
    C_OFF1 = 0x00,  // N/A
    C_NEG1 = 0x20,  // PL5

    A_POS3 = 0x01 << 8,  // PK0
    A_OFF3 = 0x00 << 8,  // N/A
    A_NEG3 = 0x02 << 8,  // PK1
    B_POS3 = 0x04 << 8,  // PK2
    B_OFF3 = 0x00 << 8,  // N/A
    B_NEG3 = 0x08 << 8,  // PK3
    C_POS3 = 0x10 << 8,  // PK4
    C_OFF3 = 0x00 << 8,  // N/A
    C_NEG3 = 0x20 << 8,  // PK5

    A_POS9 = 0x01 << 16,  // PA0
    A_OFF9 = 0x00 << 16,  // N/A
    A_NEG9 = 0x02 << 16,  // PA1
    B_POS9 = 0x04 << 16,  // PA2
    B_OFF9 = 0x00 << 16,  // N/A
    B_NEG9 = 0x08 << 16,  // PA3
    C_POS9 = 0x10 << 16,  // PA4
    C_OFF9 = 0x00 << 16,  // N/A
    C_NEG9 = 0x20 << 16,  // PA5
};

// 3 level

/*
constexpr uint8_t svm_phase_levels_a[] = {A_NEG1, A_OFF1, A_POS1};
constexpr uint8_t svm_phase_levels_b[] = {B_NEG1, B_OFF1, B_POS1};
constexpr uint8_t svm_phase_levels_c[] = {C_NEG1, C_OFF1, C_POS1};
*/

// 9 level

/*
constexpr uint8_t svm_phase_levels_a[] = {
    A_NEG3 | A_NEG1, A_NEG3 | A_OFF1, A_NEG3 | A_POS1,
    A_OFF3 | A_NEG1, A_OFF3 | A_OFF1, A_OFF3 | A_POS1,
    A_POS3 | A_NEG1, A_POS3 | A_OFF1, A_POS3 | A_POS1};
constexpr uint8_t svm_phase_levels_b[] = {
    B_NEG3 | B_NEG1, B_NEG3 | B_OFF1, B_NEG3 | B_POS1,
    B_OFF3 | B_NEG1, B_OFF3 | B_OFF1, B_OFF3 | B_POS1,
    B_POS3 | B_NEG1, B_POS3 | B_OFF1, B_POS3 | B_POS1};
constexpr uint8_t svm_phase_levels_c[] = {
    C_NEG3 | C_NEG1, C_NEG3 | C_OFF1, C_NEG3 | C_POS1,
    C_OFF3 | C_NEG1, C_OFF3 | C_OFF1, C_OFF3 | C_POS1,
    C_POS3 | C_NEG1, C_POS3 | C_OFF1, C_POS3 | C_POS1};*/

// 27 level

constexpr uint32_t svm_phase_levels_a[] = {
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
constexpr uint32_t svm_phase_levels_b[] = {
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
constexpr uint32_t svm_phase_levels_c[] = {
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
