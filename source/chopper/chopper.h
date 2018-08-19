#pragma once

#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

static const uint32_t chopper_pwm_peripheral = SYSCTL_PERIPH_PWM0;
static const uint32_t chopper_pwm_base = PWM0_BASE;
static const uint32_t chopper_gpio_peripheral = SYSCTL_PERIPH_GPIOF;
static const uint32_t chopper_gpio_port_base = GPIO_PORTF_BASE;
static const uint32_t chopper_gpio_pin_ac_pos = GPIO_PIN_0;
static const uint32_t chopper_gpio_pin_ac_neg = GPIO_PIN_1;

void start_chopper();
