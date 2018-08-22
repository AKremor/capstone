#include <source/chopper/chopper.h>
#include <source/hal/Board.h>
#include <source/hal/adc.h>
#include <source/hal/hil.h>
#include <source/mpc/mpc.h>
#include <source/quantities.h>
#include <source/reference_signal/sine_wave.h>
#include <source/svm/svm.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>
#include <xdc/runtime/System.h>
#include "arm_math.h"
#include "assert.h"

void svm_control_loop(Timer_Handle handle);

arm_pid_instance_f32 PID_d;
arm_pid_instance_f32 PID_q;

Timer_Handle system_timer;

volatile uint64_t state_counter = 0;

void init_hbridge_io() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)))
        ;

    GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(
        GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
};

void init_svm_timer() {
    Timer_init();

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = svm_timer_hz;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = svm_control_loop;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (use_svm_timer && Timer_start(timer0) == Timer_STATUS_ERROR) {
        System_abort("SVM timer did not start");
    }
};

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
    init_svm_timer();
    init_adc();

    SystemState* state = SystemState::get();
    float32_t adc_readings[2];

    while (1) {
        send_state_to_simulator();

        if (use_hil) {
            receive_state_from_simulator();
        }

        if (!use_svm_timer) {
            svm_control_loop(NULL);
        }

        // Sample ADC
        read_adc(adc_readings);
        abc_quantity quantity = {adc_readings[0] * 10, 10 * adc_readings[1], 0};
        state->load_voltage.set_abc(quantity);
    }
}

void svm_control_loop(Timer_Handle handle) {
    // Timer handle not used so can be made NULL
    SystemState* state = SystemState::get();

    // Pretend this is magically a current even though it's really a voltage
    abc_quantity value = SineWave::getValueAbc(state_counter);

    state->reference.set_abc(value);
    float32_t sinVal = sinf(2 * PI * frequency_hz * state_counter / 1000);
    float32_t cosVal = cosf(2 * PI * frequency_hz * state_counter / 1000);

    float32_t Ialpha = 0;
    float32_t Ibeta = 0;
    arm_clarke_f32(value.a, value.b, &Ialpha, &Ibeta);
    float32_t Id = 0;
    float32_t Iq = 0;
    arm_park_f32(Ialpha, Ibeta, &Id, &Iq, sinVal, cosVal);

    float32_t Ialphasense = 0;
    float32_t Ibetasense = 0;
    arm_clarke_f32(state->load_line_current.get_abc().a,
                   state->load_line_current.get_abc().b, &Ialphasense,
                   &Ibetasense);
    float32_t Idsense = 0;
    float32_t Iqsense = 0;
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

    // Anti windup
    if (PID_d.state[2] > n_levels) {
        PID_d.state[2] = n_levels;
    }
    if (PID_d.state[2] < 0) {
        PID_d.state[2] = 0;
    }

    if (PID_q.state[2] > n_levels) {
        PID_q.state[2] = n_levels;
    }
    if (PID_q.state[2] < 0) {
        PID_q.state[2] = 0;
    }

    PhaseVoltageLevel levels =
        svm_modulator(Idcontrol, Iqcontrol, sinVal, cosVal);

    state->a_phase = levels.a;
    state->b_phase = levels.b;
    state->c_phase = levels.c;

    GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[state->a_phase]);
    GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[state->b_phase]);
    GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, svm_phase_levels_c[state->c_phase]);

    state_counter++;
}
