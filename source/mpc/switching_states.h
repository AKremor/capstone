#pragma once

#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

//static constexpr int n_phases = 3;
//static constexpr int n_levels = 27;  // TODO This is per phase

class PhaseVoltageLevel {
   public:
    int a;
    int b;
    int c;
};

// TODO Make more states because 27 isn't useful. This is actually meant to be
// 81
static const PhaseVoltageLevel cell_states[27] = {
    {-1, -1, -1}, {-1, -1, 0}, {-1, -1, 1}, {-1, 0, -1}, {-1, 0, 0}, {-1, 0, 1},
    {-1, 1, -1},  {-1, 1, 0},  {-1, 1, 1},  {0, -1, -1}, {0, -1, 0}, {0, -1, 1},
    {0, 0, -1},   {0, 0, 0},   {0, 0, 1},   {0, 1, -1},  {0, 1, 0},  {0, 1, 1},
    {1, -1, -1},  {1, -1, 0},  {1, -1, 1},  {1, 0, -1},  {1, 0, 0},  {1, 0, 1},
    {1, 1, -1},   {1, 1, 0},   {1, 1, 1}};
/*

    static const PhaseVoltageLevel cell_states[n_levels] = {
    {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
    {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}};
*/

// TODO This arrangement
#define POS_C1 GPIO_PIN_0
#define POS_C2 GPIO_PIN_1
#define POS_C3 GPIO_PIN_2
#define NEG_C1 GPIO_PIN_3
#define NEG_C2 GPIO_PIN_4
#define NEG_C3 GPIO_PIN_5
#define ZERO_C1 0
#define ZERO_C2 0
#define ZERO_C3 0

const uint8_t gate_signals[n_levels] = {
    NEG_C1 | NEG_C2 | NEG_C3,   NEG_C1 | NEG_C2 | ZERO_C3,
    NEG_C1 | NEG_C2 | POS_C3,

    NEG_C1 | ZERO_C2 | NEG_C3,  NEG_C1 | ZERO_C2 | ZERO_C3,
    NEG_C1 | ZERO_C2 | POS_C3,

    NEG_C1 | POS_C2 | NEG_C3,   NEG_C1 | POS_C2 | ZERO_C3,
    NEG_C1 | POS_C2 | POS_C3,

    ZERO_C1 | NEG_C2 | NEG_C3,  ZERO_C1 | NEG_C2 | ZERO_C3,
    ZERO_C1 | NEG_C2 | POS_C3,

    ZERO_C1 | ZERO_C2 | NEG_C3, ZERO_C1 | ZERO_C2 | ZERO_C3,
    ZERO_C1 | ZERO_C2 | POS_C3,

    ZERO_C1 | POS_C2 | NEG_C3,  ZERO_C1 | POS_C2 | ZERO_C3,
    ZERO_C1 | POS_C2 | POS_C3,

    POS_C1 | NEG_C2 | NEG_C3,   POS_C1 | NEG_C2 | ZERO_C3,
    ZERO_C1 | NEG_C2 | POS_C3,

    POS_C1 | ZERO_C2 | NEG_C3,  POS_C1 | ZERO_C2 | ZERO_C3,
    POS_C1 | ZERO_C2 | POS_C3,

    POS_C1 | POS_C2 | NEG_C3,   POS_C1 | POS_C2 | ZERO_C3,
    POS_C1 | POS_C2 | POS_C3,
};
