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

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 20000 Hz

* 0 Hz - 1000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 3.666895709039203 dB

* 2000 Hz - 10000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -41.09174091632818 dB

*/

#define FILTER_TAP_NUM 27

static float32_t filter_taps[FILTER_TAP_NUM] = {
    -0.007775715121256268, -0.007938974136595613, -0.009534265788246128,
    -0.008779578259641298, -0.004381884421750879, 0.004666131585205163,
    0.0188044731228937,    0.03764144706001848,   0.05992101812383003,
    0.08357444021744635,   0.10601855702701225,   0.12454015906119098,
    0.13674393462068657,   0.14100385434561774,   0.13674393462068657,
    0.12454015906119098,   0.10601855702701225,   0.08357444021744635,
    0.05992101812383003,   0.03764144706001848,   0.0188044731228937,
    0.004666131585205163,  -0.004381884421750879, -0.008779578259641298,
    -0.009534265788246128, -0.007938974136595613, -0.007775715121256268};

static uint64_t system_time_us = 0;

arm_pid_instance_f32 PID_d;
arm_pid_instance_f32 PID_q;

arm_fir_instance_f32 FIR_IAa;
arm_fir_instance_f32 FIR_IBb;
arm_fir_instance_f32 FIR_ICc;

uint16_t adc_s1_fire = 0;
uint16_t adc_s2_fire = 0;
uint16_t adc_s3_fire = 0;

float I_Aa, I_Bb, I_Cc;
float V_an, V_bn, V_cn;

float adc_raw_voltages[6];

SystemState* state = SystemState::get();
PhaseVoltageLevel duty_levels[3] = {0, 0, 0};
uint32_t levels_all[3] = {0, 0, 0};
float32_t duty_cycles[3] = {0.5, 0.25, 0.25};

float32_t prev_adc[3] = {0, 0, 0};

float32_t current_history[3][3] = {};
uint32_t current_history_index = 0;

float Id_ref = magnitude;
float Iq_ref = 0.0;

float Id_error, Iq_error;

uint8_t level_9_detect, level_3_detect, level_1_detect;

SystemStatus system_state = START;

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

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                                   GPIO_PIN_2 | GPIO_PIN_3 |
                                                   GPIO_PIN_4 | GPIO_PIN_5);

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                                   GPIO_PIN_2 | GPIO_PIN_3 |
                                                   GPIO_PIN_4 | GPIO_PIN_5);

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                                   GPIO_PIN_2 | GPIO_PIN_3 |
                                                   GPIO_PIN_4 | GPIO_PIN_5);

    // Sense pins
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_6);
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_6);

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

    // MAP_TimerConfigure(TIMER4_BASE, TIMER_CFG_A_ONE_SHOT);
    // MAP_TimerPrescaleSet(TIMER4_BASE, TIMER_A, 120);
    // MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, 65000);
    // MAP_TimerADCEventSet(TIMER4_BASE, TIMER_ADC_TIMEOUT_A);
    // MAP_TimerControlTrigger(TIMER4_BASE, TIMER_A, true);
    // MAP_TimerEnable(TIMER4_BASE, TIMER_A);
}

float32_t state_IAa[FILTER_TAP_NUM];
float32_t state_IBb[FILTER_TAP_NUM];
float32_t state_ICc[FILTER_TAP_NUM];

void mainThread(void* arg0) {
    PID_d.Kd = Kd;
    PID_d.Ki = Ki;
    PID_d.Kp = Kp;
    arm_pid_init_f32(&PID_d, 1);
    PID_q.Kd = Kd;
    PID_q.Ki = Ki;
    PID_q.Kp = Kp;
    arm_pid_init_f32(&PID_q, 1);

    arm_fir_init_f32(&FIR_IAa, FILTER_TAP_NUM, filter_taps, state_IAa, 1);
    arm_fir_init_f32(&FIR_IBb, FILTER_TAP_NUM, filter_taps, state_IBb, 1);
    arm_fir_init_f32(&FIR_ICc, FILTER_TAP_NUM, filter_taps, state_ICc, 1);

    start_chopper();
    init_hil();
    init_hbridge_io();
    init_adc();

    while (0) {
        for (int i = 0; i < n_levels; i++) {
            uint32_t lev = svm_phase_levels_a[i] | svm_phase_levels_b[i] |
                           svm_phase_levels_c[i];

            MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, lev >> 0 & 0xFF);
            MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, lev >> 8 & 0xFF);
            MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, lev >> 16 & 0xFF);
            SysCtlDelay(120E6 * 0.001);
        }
    }

    init_timers();

    while (1) {
    }
}

void TIMER0A_IRQHandler(void) {
    // PWM 1
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, GPIO_PIN_1);

    // Set the 1V levels, they are located [7:0]
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, levels_all[0] >> 0 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, levels_all[0] >> 8 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, levels_all[0] >> 16 & 0xFF);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s1_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, 0);
}

void TIMER1A_IRQHandler(void) {
    // PWM 2
    MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_PIN_2);

    // Set the 1V levels, they are located [7:0]
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, levels_all[1] >> 0 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, levels_all[1] >> 8 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, levels_all[1] >> 16 & 0xFF);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s2_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, 0);
}

void TIMER2A_IRQHandler(void) {
    // PWM 3
    MAP_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_3, GPIO_PIN_3);

    // Set the 1V levels, they are located [7:0]
    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, levels_all[2] >> 0 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, levels_all[2] >> 8 & 0xFF);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, levels_all[2] >> 16 & 0xFF);

    MAP_TimerLoadSet(TIMER4_BASE, TIMER_A, adc_s3_fire);
    MAP_TimerEnable(TIMER4_BASE, TIMER_A);

    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_3, 0);
}

void TIMER3A_IRQHandler(void) {
    // SVM Loop
    MAP_TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_PIN_0);
    svm_control_loop();
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);
}

void svm_control_loop() {
    // Check if anyone dis/connected anything
    // These are active low (low signals the cell is connected)
    level_9_detect = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) ? 0 : 1;
    level_3_detect = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_6) ? 0 : 1;
    level_1_detect = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_6) ? 0 : 1;

    system_time_us += svm_period_us;

    float32_t sinVal, cosVal;
    arm_sin_cos_f32(360 * fundamental_frequency_hz * system_time_us * 1E-6,
                    &sinVal, &cosVal);

    volatile float32_t Id = Id_ref;
    volatile float32_t Iq = Iq_ref;

    volatile float32_t Ialphasense = 0, Ibetasense = 0;
    arm_clarke_f32(I_Aa, I_Bb, (float32_t*)&Ialphasense,
                   (float32_t*)&Ibetasense);
    volatile float32_t Idsense = 0, Iqsense = 0;
    arm_park_f32(Ialphasense, Ibetasense, (float32_t*)&Idsense,
                 (float32_t*)&Iqsense, sinVal, cosVal);

    Id_error = Id - Idsense;
    Iq_error = Iq - Iqsense;
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

    levels_all[0] = svm_phase_levels_a[duty_levels[0].a] |
                    svm_phase_levels_b[duty_levels[0].b] |
                    svm_phase_levels_c[duty_levels[0].c];

    levels_all[1] = svm_phase_levels_a[duty_levels[1].a] |
                    svm_phase_levels_b[duty_levels[1].b] |
                    svm_phase_levels_c[duty_levels[1].c];

    levels_all[2] = svm_phase_levels_a[duty_levels[2].a] |
                    svm_phase_levels_b[duty_levels[2].b] |
                    svm_phase_levels_c[duty_levels[2].c];

    state->duty_cycles[0] = duty_cycles[0];
    state->duty_cycles[1] = duty_cycles[1];
    state->duty_cycles[2] = duty_cycles[2];

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

    if (system_state == START || system_state == STEP) {
        if (system_state == STEP) {
            system_state == PAUSE;
        }
        MAP_TimerEnable(TIMER0_BASE, TIMER_A);
        MAP_TimerEnable(TIMER1_BASE, TIMER_A);
        MAP_TimerEnable(TIMER2_BASE, TIMER_A);
    }
    send_state_to_simulator();

    adcReadChannels(adc_raw_voltages);

    // Now convert and store them into globals
    float32_t I_Aa_t = 2 * (adc_raw_voltages[2] - 1.65);
    float32_t I_Bb_t = 2 * (adc_raw_voltages[1] - 1.65);
    float32_t I_Cc_t = 2 * (adc_raw_voltages[0] - 1.65);

    arm_fir_f32(&FIR_IAa, &I_Aa_t, &I_Aa, 1);
    arm_fir_f32(&FIR_IBb, &I_Bb_t, &I_Bb, 1);
    arm_fir_f32(&FIR_ICc, &I_Cc_t, &I_Cc, 1);

    V_an = 3 * adc_raw_voltages[5];
    V_bn = 3 * adc_raw_voltages[4];
    V_cn = 3 * adc_raw_voltages[3];

    receive_uart();
}

// void ADC0SS2_IRQHandler(void) {}
