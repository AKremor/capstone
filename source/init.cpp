#include "init.h"
#include <source/chopper/chopper.h>
#include <source/hal/adc.h>
#include <source/hal/hil.h>
#include <source/quantities.h>
#include <source/reference_signal/sine_wave.h>
#include <source/svm/svm.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

static uint64_t system_time_us = 0;

arm_pid_instance_f32 PID_d;
arm_pid_instance_f32 PID_q;

uint16_t adc_s1_fire = 0;
uint16_t adc_s2_fire = 0;
uint16_t adc_s3_fire = 0;

void init_hbridge_io() {
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)))
        ;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)))
        ;

    MAP_GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    MAP_GPIOPinTypeGPIOOutput(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    MAP_GPIOPinTypeGPIOOutput(
        GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    MAP_GPIOPinTypeGPIOOutput(
        GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
};

void init_timers() {
    // All timers are configured to have a tick period of 1us

    // Timer 0 -> PWM1
    // Timer 1 -> PWM2
    // Timer 2 -> PWM3
    // Timer 2 -> SVM

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4))) {
    }

    MAP_TimerConfigure(TIMER0_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER0_BASE, TIMER_A, 120);
    MAP_IntEnable(INT_TIMER0A);

    MAP_TimerConfigure(TIMER1_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT);
    MAP_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER1_BASE, TIMER_A, 120);
    MAP_IntEnable(INT_TIMER1A);

    MAP_TimerConfigure(TIMER2_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_ONE_SHOT);
    MAP_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER2_BASE, TIMER_A, 120);
    MAP_IntEnable(INT_TIMER2A);

    MAP_TimerConfigure(TIMER3_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    MAP_TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER3_BASE, TIMER_A, 120);
    MAP_TimerLoadSet(TIMER3_BASE, TIMER_A, svm_period_us);
    MAP_IntEnable(INT_TIMER3A);
    MAP_TimerEnable(TIMER3_BASE, TIMER_A);

    MAP_TimerConfigure(TIMER4_BASE, TIMER_CFG_A_ONE_SHOT);
    MAP_TimerPrescaleSet(TIMER4_BASE, TIMER_A, 120);
    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, 65000);
    MAP_TimerADCEventSet(TIMER4_BASE, TIMER_ADC_TIMEOUT_A);
    MAP_TimerControlTrigger(TIMER4_BASE, TIMER_A, true);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);
}

void mainThread(void* arg0) {
    PID_d.Kd = Kd;
    PID_d.Ki = Ki;
    PID_d.Kp = Kp;
    arm_pid_init_f32(&PID_d, 1);
    PID_q.Kd = Kd;
    PID_q.Ki = Ki;
    PID_q.Kp = Kp;
    arm_pid_init_f32(&PID_q, 1);

    start_chopper();
    init_hil();
    init_hbridge_io();
    init_adc();
    init_timers();

    while (1) {
    }
}

SystemState* state = SystemState::get();
PhaseVoltageLevel duty_levels[3] = {0, 0, 0};
float32_t duty_cycles[3] = {0.5, 0.25, 0.25};

void TIMER0A_IRQHandler(void) {
    // PWM 1
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_PIN_0);

    PhaseVoltageLevel levels = duty_levels[0];
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[levels.a]);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[levels.b]);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, svm_phase_levels_c[levels.c]);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s1_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
}

void TIMER1A_IRQHandler(void) {
    // PWM 2
    MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, GPIO_PIN_1);

    PhaseVoltageLevel levels = duty_levels[1];
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[levels.a]);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[levels.b]);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, svm_phase_levels_c[levels.c]);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s2_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, 0);
}

void TIMER2A_IRQHandler(void) {
    // PWM 3
    MAP_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_PIN_2);

    PhaseVoltageLevel levels = duty_levels[2];
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[levels.a]);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[levels.b]);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, svm_phase_levels_c[levels.c]);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s3_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, 0);
}

void TIMER3A_IRQHandler(void) {
    MAP_TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
    svm_control_loop();
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0);
}

float32_t prev_adc[3] = {0, 0, 0};

float32_t current_history[3][3] = {};
uint32_t current_history_index = 0;

void svm_control_loop() {
    system_time_us += svm_period_us;

    float32_t sinVal, cosVal;
    arm_sin_cos_f32(omega * system_time_us, &sinVal, &cosVal);


    // Bias current readings appropriately
     abc_quantity quantity_current = {
        (current_history[1][1] + current_history[2][1] +
         current_history[3][1]) /
            3,
        (current_history[1][2] + current_history[2][2] +
         current_history[3][2]) /
            3,
        (current_history[1][3] + current_history[2][3] +
         current_history[3][3]) /
            3,
    };

    volatile abc_quantity reference_value = SineWave::getValueAbc(system_time_us);

    volatile float32_t Ialpha = 0, Ibeta = 0;
    arm_clarke_f32(reference_value.a, reference_value.b, (float32_t*)&Ialpha, (float32_t*)&Ibeta);

    volatile float32_t Id = 0, Iq = 0;
    arm_park_f32(Ialpha, Ibeta, (float32_t*)&Id, (float32_t*)&Iq, sinVal, cosVal);

    abc_quantity load_line_current = quantity_current;

    volatile float32_t Ialphasense = 0, Ibetasense = 0;
    arm_clarke_f32(load_line_current.a, load_line_current.b, (float32_t*)&Ialphasense,
                   (float32_t*) &Ibetasense);

    volatile float32_t Idsense = 0, Iqsense = 0;
    arm_park_f32(Ialphasense, Ibetasense, (float32_t*)&Idsense, (float32_t*)&Iqsense, sinVal, cosVal);

    volatile dq0_quantity pid_error = {Id - Idsense, Iq - Iqsense, 0};

    volatile float32_t Idcontrol, Iqcontrol;
    if (use_closed_loop) {
        Idcontrol = arm_pid_f32(&PID_d, pid_error.d);
        Iqcontrol = arm_pid_f32(&PID_q, pid_error.q);
    } else {
        Idcontrol = Id;
        Iqcontrol = Iq;
    }

    svm_modulator(Idcontrol, Iqcontrol, sinVal, cosVal, duty_levels,
                  duty_cycles);

    // Load up the three state configs
    adc_s1_fire = duty_cycles[0] * pwm_period_us / 2;
    adc_s2_fire = duty_cycles[1] * pwm_period_us / 2;
    adc_s3_fire = duty_cycles[2] * pwm_period_us / 2;

    // The indexing here intentionally looks wrong
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, 0);
    MAP_TimerLoadSet(TIMER1_BASE, TIMER_A, duty_cycles[0] * pwm_period_us);
    MAP_TimerLoadSet(TIMER2_BASE, TIMER_A,
                     (duty_cycles[0] + duty_cycles[1]) * pwm_period_us);
    MAP_TimerLoadSet(TIMER3_BASE, TIMER_A, svm_period_us - 25);

    MAP_TimerEnable(TIMER0_BASE, TIMER_A);
    MAP_TimerEnable(TIMER1_BASE, TIMER_A);
    MAP_TimerEnable(TIMER2_BASE, TIMER_A);
}

void ADC0SS2_IRQHandler(void) {
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);

    uint32_t adc_digital_value[3];

    // TODO Produce the actual float values

    MAP_ADCIntClear(ADC0_BASE, 2);
    MAP_ADCSequenceDataGet(ADC0_BASE, 2, adc_digital_value);

    current_history[current_history_index][0] =
        2 * (convertAdjustedSingle(adc_digital_value[0]) - 1.6f);
    current_history[current_history_index][1] =
        2 * (convertAdjustedSingle(adc_digital_value[1]) - 1.6f);
    current_history[current_history_index][2] =
        2 * (convertAdjustedSingle(adc_digital_value[2]) - 1.6f);

    current_history_index = (current_history_index + 1) % 3;

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, 0);
}
