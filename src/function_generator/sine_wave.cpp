#include <src/hal/Board.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/Timer.h>

const uint8_t sine_table[] = {255, 254, 246, 234, 219, 199, 177, 153,
                              128, 103, 79,  57,  37,  22,  10,  2,
                              1,   2,   10,  22,  37,  57,  79,  103,
                              128, 153, 177, 199, 219, 234, 246, 255};

void updatePwmValue(Timer_Handle timer);
PWM_Handle pwm1;

void initSineWaveOutput() {
    /*pwm1 = NULL;
    PWM_Params pwm_params;

    PWM_init();

    PWM_Params_init(&pwm_params);
    pwm_params.dutyUnits = PWM_DUTY_COUNTS;
    pwm_params.dutyValue = 0;
    pwm_params.periodUnits = PWM_PERIOD_COUNTS;
    pwm_params.periodValue = 255;
    pwm1 = PWM_open(Board_PWM0, &pwm_params);
    */
    if (pwm1 == NULL) {
        /* Board_PWM0 did not open */
        while (1)
            ;
    }

    PWM_start(pwm1);

    Timer_init();

    Timer_Handle timer;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period =
        1000 * 8 *
        4;  // Sets the frequency of the sine wave, scaled by samples per wave
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = updatePwmValue;

    timer = Timer_open(Board_TIMER1, &params);

    if (timer == NULL) {
        /* Failed to initialized timer */
        while (1)
            ;
    }

    if (Timer_start(timer) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1)
            ;
    }
}

uint8_t sine_table_index = 0;
void updatePwmValue(Timer_Handle timer) {
    PWM_setDuty(pwm1, sine_table[sine_table_index % 32]);
    sine_table_index++;
}
