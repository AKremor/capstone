#ifndef SRC_SYSTEM_CONFIG_H_
#define SRC_SYSTEM_CONFIG_H_

#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

static const uint32_t chopper_pwm_peripheral = SYSCTL_PERIPH_PWM0;
static const uint32_t chopper_pwm_base = PWM0_BASE;
static const uint32_t chopper_gpio_peripheral = SYSCTL_PERIPH_GPIOF;
static const uint32_t chopper_gpio_port_base = GPIO_PORTF_BASE;
static const uint32_t chopper_gpio_pin_ac_pos = GPIO_PIN_0;
static const uint32_t chopper_gpio_pin_ac_neg = GPIO_PIN_1;

constexpr uint32_t system_clock_hz = 120e6;

static const uint32_t chopper_hz = 10000;
static const uint32_t chopper_rising_dead_band_ns = 3600;
static const uint32_t chopper_falling_dead_band_ns = 3600;

// 12000, 1000, 1000 for 10kHz
// 6000, 672, 672 for 20kHz
// 3000, 1000, 1000 for 40kHz

static constexpr uint16_t svm_frequency_hz = 100;

#endif /* SRC_SYSTEM_CONFIG_H_ */
