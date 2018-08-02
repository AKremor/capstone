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
static const uint16_t chopper_period = 6000;

static const uint16_t chopper_rising_dead_band_width = 0;
static const uint16_t chopper_falling_dead_band_width = 0;

// PL0 9VL POS
// PL1 9VR NEG
// PL2 3VL POS
// PL3 3VR NEG

// Port L
#define POS9 0x01
#define NEG9 0x02
#define POS3 0x04
#define NEG3 0x08
#define OFF9 0x00
#define OFF3 0x00

#endif /* SRC_SYSTEM_CONFIG_H_ */
