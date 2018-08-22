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

struct ab0_quantity {
    float32_t a;
    float32_t b;
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
    ab0_quantity get_ab0() {
        ab0_quantity value = {0, 0, 0};

        value.a = 2.0 / 3 * this->quantity.a - 1.0 / 3 * this->quantity.b -
                  1.0 / 3 * this->quantity.c;
        value.b =
            1.0 / sqrt(3) * this->quantity.b - 1.0 / sqrt(3) * this->quantity.c;
        value.zero = 1.0 / 3 * this->quantity.a + 1.0 / 3 * this->quantity.b +
                     1.0 / 3 * this->quantity.c;
        return value;
    }
    dq0_quantity get_dq0(float32_t wt) {
        // TODO
        dq0_quantity remove = {0, 0, 0};
        return remove;
    }
    gh_quantity get_gh(float32_t Vdc) {
        gh_quantity gh_value;
        gh_value.g =
            1 / (3 * Vdc) *
            (2 * this->quantity.a - this->quantity.b - this->quantity.c);
        gh_value.h =
            1 / (3 * Vdc) *
            (-1 * this->quantity.a + 2 * this->quantity.b - this->quantity.c);

        return gh_value;
    }
    void set_abc(abc_quantity quantity) { this->quantity = quantity; }
    void set_ab0(ab0_quantity quantity) {
        this->quantity.a = quantity.a + quantity.zero;
        this->quantity.b =
            -0.5 * quantity.a + sqrt(3) / 2 * quantity.b + quantity.zero;
        this->quantity.c =
            -0.5 * quantity.a - sqrt(3) / 2 * quantity.b + quantity.zero;
    }
    void set_dq0(dq0_quantity quantity, float32_t wt) {
        // TODO
    }
    void set_gh(gh_quantity quantity) {
        // TODO
        // Unsure if the gh quantity is easily invertible?
        assert(0);
    }

   private:
    abc_quantity quantity;
};
