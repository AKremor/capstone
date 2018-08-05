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

// N = (1 / f) * SysClk.  Where N is the function parameter, f is the
// desired frequency, and SysClk is the system clock frequency. Note that
// the maximum period you can set is 2^16 - 1.
// Configure for 20kHz on a 120MHz base timer
static const uint16_t chopper_period = 12000;

static const uint16_t chopper_rising_dead_band_width = 1000;
static const uint16_t chopper_falling_dead_band_width = 1000;

static constexpr uint16_t svm_frequency_hz = 1;

#endif /* SRC_SYSTEM_CONFIG_H_ */
