#pragma once
#include "arm_math.h"
#include "assert.h"

struct abc_quantity {
    float32_t a;
    float32_t b;
    float32_t c;
};

struct dq0_quantity {
    float32_t d;
    float32_t q;
    float32_t zero;
};

struct gh_quantity {
    float32_t g;
    float32_t h;
};

class PhaseVoltageLevel {
   public:
    int32_t a;
    int32_t b;
    int32_t c;
};

class three_phase_quantity {
   public:
    abc_quantity get_abc() { return quantity; }
    void set_abc(abc_quantity quantity) { this->quantity = quantity; }
    abc_quantity quantity;
};
