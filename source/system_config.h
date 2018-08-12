#ifndef SRC_SYSTEM_CONFIG_H_
#define SRC_SYSTEM_CONFIG_H_

#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

static const uint32_t chopper_pwm_peripheral = SYSCTL_PERIPH_PWM0;
static const uint32_t chopper_pwm_base = PWM0_BASE;
static const uint32_t chopper_gpio_peripheral = SYSCTL_PERIPH_GPIOF;
static const uint32_t chopper_gpio_port_base = GPIO_PORTF_BASE;
static const uint32_t chopper_gpio_pin_ac_pos = GPIO_PIN_0;
static const uint32_t chopper_gpio_pin_ac_neg = GPIO_PIN_1;

constexpr uint32_t system_clock_mhz = 120;

static const uint32_t chopper_hz = 10000;
static const uint32_t chopper_rising_dead_band_ns = 3600;
static const uint32_t chopper_falling_dead_band_ns = 3600;

// 12000, 1000, 1000 for 10kHz
// 6000, 672, 672 for 20kHz
// 3000, 1000, 1000 for 40kHz

// Internal reference generation
static constexpr uint16_t svm_timer_hz = 100;
static constexpr uint32_t n_levels = 9;
static constexpr float32_t Vdc = 1;
static constexpr float32_t magnitude = 0.8 * 0.866 * 1.141 * ((n_levels - 0) / 1.0) * Vdc;
static constexpr float32_t frequency_hz = 20;

#endif /* SRC_SYSTEM_CONFIG_H_ */
