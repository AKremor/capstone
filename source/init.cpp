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

static uint32_t system_time = 0;

arm_pid_instance_f32 PID_d;
arm_pid_instance_f32 PID_q;

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

void SysTick_Handler(void) {
    system_time++;
    system_time = system_time % omega_period;
}

void init_timers() {
    // Configure the system tick timer at 1us for timing
    MAP_SysTickPeriodSet(120);  // Timing in 1us
    MAP_SysTickIntEnable();
    MAP_SysTickEnable();

    // All timers are configured to have a tick period of 1us

    // Timer 0 -> PWM
    // Timer 1 -> SVM
    // Timer 2 -> ADC (if not in SVM loop)

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))) {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2))) {
    }

    MAP_TimerConfigure(TIMER0_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER0_BASE, TIMER_A, 120);
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, 1);
    MAP_IntEnable(INT_TIMER0A);
    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

    MAP_TimerConfigure(TIMER1_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    MAP_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER1_BASE, TIMER_A, 120);
    MAP_TimerLoadSet(TIMER1_BASE, TIMER_A, svm_period_us);
    MAP_IntEnable(INT_TIMER1A);
    MAP_TimerEnable(TIMER1_BASE, TIMER_A);

    MAP_TimerConfigure(TIMER2_BASE,
                       TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    MAP_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerPrescaleSet(TIMER2_BASE, TIMER_A, 120);
    MAP_TimerLoadSet(TIMER2_BASE, TIMER_A, adc_period_us);
    MAP_IntEnable(INT_TIMER2A);
    MAP_TimerEnable(TIMER2_BASE, TIMER_A);
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
static int duty_state_counter = 0;
PhaseVoltageLevel duty_levels_current[3] = {0, 0, 0};
PhaseVoltageLevel duty_levels_next[3] = {0, 0, 0};

float32_t duty_cycles_current[3] = {1, 1, 1};
float32_t duty_cycles_next[3] = {1, 1, 1};

void TIMER0A_IRQHandler(void) {
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_PIN_5);

    // TODO we will run junk data for the very first sample period Ts
    if (duty_state_counter == 0) {
        memcpy(&duty_levels_current, &duty_levels_next,
               3 * sizeof(PhaseVoltageLevel));
        memcpy(&duty_cycles_current, &duty_cycles_next, 3 * sizeof(float32_t));
    }

    state->a_phase = duty_levels_current[duty_state_counter].a;
    state->b_phase = duty_levels_current[duty_state_counter].b;
    state->c_phase = duty_levels_current[duty_state_counter].c;

    MAP_GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[state->a_phase]);
    MAP_GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[state->b_phase]);
    MAP_GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, svm_phase_levels_c[state->c_phase]);

    uint16_t val = duty_cycles_current[duty_state_counter] * pwm_period_us;
    // Configure the timer for next go
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, val);

    duty_state_counter = (duty_state_counter + 1) % 3;
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0);
}

void TIMER1A_IRQHandler(void) {
    MAP_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
    svm_control_loop();
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, 0);
}

void TIMER2A_IRQHandler(void) {
    MAP_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
    MAP_GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, 0);
    // TODO(akremor): Put current readings here
}

float32_t prev_adc[5];

void svm_control_loop() {
    float32_t sinVal, cosVal;
    arm_sin_cos_f32(omega * system_time, &sinVal, &cosVal);

    float32_t adc_readings[5];
    read_adc(adc_readings);

    // Bias current readings appropriately
    abc_quantity quantity_current = {
        (float32_t)(2 * ((adc_readings[0] + prev_adc[0]) / 2.0 - 1.6f)),
        (float32_t)(2 * ((adc_readings[1] + prev_adc[1]) / 2.0 - 1.6f)),
        (float32_t)(2 * ((adc_readings[2] + prev_adc[2]) / 2.0 - 1.6f))};
    memcpy(prev_adc, adc_readings, 5 * sizeof(float32_t));

    abc_quantity reference_value = SineWave::getValueAbc(system_time);

    float32_t Ialpha = 0, Ibeta = 0;
    arm_clarke_f32(reference_value.a, reference_value.b, &Ialpha, &Ibeta);

    float32_t Id = 0, Iq = 0;
    arm_park_f32(Ialpha, Ibeta, &Id, &Iq, sinVal, cosVal);

    abc_quantity load_line_current = quantity_current;

    float32_t Ialphasense = 0, Ibetasense = 0;
    arm_clarke_f32(load_line_current.a, load_line_current.b, &Ialphasense,
                   &Ibetasense);

    float32_t Idsense = 0, Iqsense = 0;
    arm_park_f32(Ialphasense, Ibetasense, &Idsense, &Iqsense, sinVal, cosVal);

    dq0_quantity pid_error = {Id - Idsense, Iq - Iqsense, 0};

    float32_t Idcontrol, Iqcontrol;
    if (use_closed_loop) {
        Idcontrol = arm_pid_f32(&PID_d, pid_error.d);
        Iqcontrol = arm_pid_f32(&PID_q, pid_error.q);
    } else {
        Idcontrol = Id;
        Iqcontrol = Iq;
    }

    PhaseVoltageLevel levels[3];
    float32_t duty_cycle[3];
    svm_modulator(Idcontrol, Iqcontrol, sinVal, cosVal, levels, duty_cycle);

    memcpy(duty_cycles_next, duty_cycle, 3 * sizeof(float32_t));
    memcpy(duty_levels_next, levels, 3 * sizeof(PhaseVoltageLevel));
}
