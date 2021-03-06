#include <source/chopper/chopper.h>
#include <source/system_config.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

void start_chopper() {
    MAP_SysCtlPeripheralEnable(chopper_pwm_peripheral);
    while (!(MAP_SysCtlPeripheralReady(chopper_pwm_peripheral)))
        ;

    /* Set the PWM clock to the system clock. */
    MAP_PWMClockSet(chopper_pwm_base, PWM_SYSCLK_DIV_1);

    /* Enable the clock to the GPIO Port F for PWM pins */
    MAP_SysCtlPeripheralEnable(chopper_gpio_peripheral);
    while (!MAP_SysCtlPeripheralReady(chopper_gpio_peripheral))
        ;

    MAP_GPIOPinConfigure(GPIO_PF0_M0PWM0);
    MAP_GPIOPinConfigure(GPIO_PF1_M0PWM1);
    MAP_GPIOPinTypePWM(chopper_gpio_port_base,
                       (chopper_gpio_pin_ac_pos | chopper_gpio_pin_ac_neg));

    /* Configure the PWM0 to count up/down without synchronization. */
    MAP_PWMGenConfigure(
        chopper_pwm_base, PWM_GEN_0,
        PWM_GEN_MODE_DBG_RUN | PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    update_frequency(chopper_hz);
}

void update_frequency(uint32_t frequency) {
    // N = (1 / f) * SysClk.  Where N is the function parameter, f is the
    // desired frequency, and SysClk is the system clock frequency. Note that
    // the maximum period you can set is 2^16 - 1.
    MAP_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0,
                        1.0 * system_clock_mhz * 1e6 / frequency);

    // Duty cycle set as a period of counts
    MAP_PWMPulseWidthSet(chopper_pwm_base, PWM_OUT_0,
                         MAP_PWMGenPeriodGet(chopper_pwm_base, PWM_GEN_0) / 2);
    MAP_PWMPulseWidthSet(chopper_pwm_base, PWM_OUT_1,
                         MAP_PWMGenPeriodGet(chopper_pwm_base, PWM_GEN_0) / 2);

    MAP_PWMOutputInvert(chopper_pwm_base, PWM_OUT_0_BIT | PWM_OUT_1_BIT, false);

    // Deadbanding also handles the complementary output
    // With the deadband width set to 0 we have standard complementary outputs
    // Configured as clock ticks so need to convert from a time duration
    MAP_PWMDeadBandEnable(
        chopper_pwm_base, PWM_GEN_0,
        chopper_rising_dead_band_ns * 10e-3 * system_clock_mhz,
        chopper_falling_dead_band_ns * 10e-3 * system_clock_mhz);

    /* Enable the PWM0 Bit 0 (PF0) and Bit 1 (PF1) output signals. */
    MAP_PWMOutputState(chopper_pwm_base, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);

    /* Enables the counter for a PWM generator block. */
    MAP_PWMGenEnable(chopper_pwm_base, PWM_GEN_0);
}
