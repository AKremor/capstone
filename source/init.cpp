#include <source/chopper/chopper.h>
#include <source/hal/Board.h>
#include <source/hal/adc.h>
#include <source/reference_signal/sine_wave.h>
#include <source/system_config.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART.h>
#include <xdc/runtime/System.h>
#include "arm_math.h"
#include "assert.h"

void svm_timer_callback(Timer_Handle handle);

arm_pid_instance_f32 PIDa;
float32_t pid_errora;

arm_pid_instance_f32 PIDb;
float32_t pid_errorb;

int8_t buffer[20];
Timer_Handle system_timer;
UART_Handle uart;

int8_t read_buffer[20];

abc_quantity load_voltage = {0, 0, 0};
abc_quantity load_line_current = {0, 0, 0};
abc_quantity load_ll_voltage = {0, 0, 0};

volatile uint64_t state_counter = 0;

void init_board() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
};

void uartcallback(UART_Handle handle, void *buf, size_t count) {}

void init_uart() {
    UART_init();

    UART_Params uart_params;
    UART_Params_init(&uart_params);
    uart_params.readEcho = UART_ECHO_OFF;
    uart_params.readDataMode = UART_DATA_BINARY;
    uart_params.writeDataMode = UART_DATA_BINARY;
    uart_params.baudRate = 921600;
    uart_params.readMode = UART_MODE_BLOCKING;
    uart_params.writeMode = UART_MODE_CALLBACK;
    uart_params.writeCallback = uartcallback;

    uart = UART_open(0, &uart_params);
}

void init_timers() {
    Timer_init();

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = svm_timer_hz;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = svm_timer_callback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        System_abort("SVM timer did not start");
    }

    Timer_Params system_timer_params;

    Timer_Params_init(&system_timer_params);
    system_timer_params.period = 2 ^ 32 - 1;
    system_timer_params.periodUnits = Timer_PERIOD_COUNTS;
    system_timer_params.timerMode = Timer_FREE_RUNNING;

    system_timer = Timer_open(Board_TIMER1, &system_timer_params);

    if (Timer_start(system_timer) == Timer_STATUS_ERROR) {
        System_abort("System timer did not start");
    }
};

void mainThread(void *arg0) {
    PIDa.Kd = Kd;
    PIDa.Ki = Ki;
    PIDa.Kp = Kp;
    arm_pid_init_f32(&PIDa, 1);
    PIDb.Kd = Kd;
    PIDb.Ki = Ki;
    PIDb.Kp = Kp;
    arm_pid_init_f32(&PIDb, 1);

    start_chopper();
    init_uart();
    init_board();
    init_timers();
    init_adc();

    // Update the system state from uart

    while (1) {
        int8_t header[1] = {NULL};

        bool has_sync1 = false;

        while (!has_sync1) {
            if (UART_read(uart, header, 1)) {
                if (header[0] == 'A') {
                    has_sync1 = true;
                }
            }
        }

        bool has_sync2 = false;

        while (!has_sync2) {
            if (UART_read(uart, header, 1)) {
                if (header[0] == 'a') {
                    has_sync2 = true;
                }
            }
        }

        // Sync'd
        UART_read(uart, read_buffer, 9);

        load_voltage = {(float32_t)read_buffer[0], (float32_t)read_buffer[1],
                        (float32_t)read_buffer[2]};
        load_line_current = {(float32_t)(read_buffer[3] / 64.0),
                             (float32_t)(read_buffer[4] / 64.0),
                             (float32_t)(read_buffer[5] / 64.0)};
        load_ll_voltage = {(float32_t)read_buffer[6], (float32_t)read_buffer[7],
                           (float32_t)read_buffer[8]};
    }
}

int32_t a_phase, b_phase, c_phase;
void svm_timer_callback(Timer_Handle handle) {
    // Pretend this is magically a current even though it's really a voltage
    abc_quantity value = SineWave::getValueAbc(state_counter);

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
    arm_clarke_f32(load_line_current.a, load_line_current.b, &Ialphasense,
                   &Ibetasense);
    float32_t Idsense = 0;
    float32_t Iqsense = 0;
    arm_park_f32(Ialphasense, Ibetasense, &Idsense, &Iqsense, sinVal, cosVal);

    pid_errora = Id - Idsense;
    pid_errorb = Iq - Iqsense;
    float32_t Idcontrol = arm_pid_f32(&PIDa, pid_errora);
    float32_t Iqcontrol = arm_pid_f32(&PIDb, pid_errorb);

    float32_t Ialphacontrol, Ibetacontrol;
    arm_inv_park_f32(Idcontrol, Iqcontrol, &Ialphacontrol, &Ibetacontrol,
                     sinVal, cosVal);

    float32_t Ia, Ib;
    arm_inv_clarke_f32(Ialphacontrol, Ibetacontrol, &Ia, &Ib);

    value.a = Ia;
    value.b = Ib;
    value.c = -1 * (Ia + Ib);

    gh_quantity hex_value;
    hex_value.g = 1 / (3 * Vdc) * (2 * value.a - value.b - value.c);
    hex_value.h = 1 / (3 * Vdc) * (-1 * value.a + 2 * value.b - value.c);

    // Ordered ul, lu, uu, ll
    gh_quantity nodes[4] = {NULL};

    nodes[0] = {ceilf(hex_value.g), floorf(hex_value.h)};
    nodes[1] = {floorf(hex_value.g), ceilf(hex_value.h)};
    nodes[2] = {ceilf(hex_value.g), ceilf(hex_value.h)};
    nodes[3] = {floorf(hex_value.g), floorf(hex_value.h)};

    // Ordered ul, lu, uu, ll
    float32_t distances[4] = {NULL};

    // Calculate nearest node
    distances[0] =
        fabs(nodes[0].g - hex_value.g) + fabs(nodes[0].h - hex_value.h);
    distances[1] =
        fabs(nodes[1].g - hex_value.g) + fabs(nodes[1].h - hex_value.h);
    distances[2] =
        fabs(nodes[2].g - hex_value.g) + fabs(nodes[2].h - hex_value.h);
    distances[3] =
        fabs(nodes[3].g - hex_value.g) + fabs(nodes[3].h - hex_value.h);

    float32_t min_distance = 10000;
    int min_index = 0;
    for (int i = 0; i < 4; i++) {
        if (distances[i] < min_distance) {
            min_distance = distances[i];
            min_index = i;
        }
    }

    gh_quantity nearest_1 = nodes[min_index];

    // Now we need to find an available voltage state.
    // This is a very rudimentary implementation

    int32_t k = 0;
    bool constraints_satisfied = false;
    int32_t g = nearest_1.g;
    int32_t h = nearest_1.h;
    int32_t n = n_levels;
    while (!constraints_satisfied) {
        if (k >= 0 && k - g >= 0 && k - g - h >= 0 && k <= n - 1 &&
            k - g <= n - 1 && k - g - h <= n - 1) {
            constraints_satisfied = true;
            break;
        }
        if (k >= n - 1) {
            // Saturation/error in modulator
            break;
        }
        k++;
    }

    a_phase = k;
    b_phase = k - nearest_1.g;
    c_phase = k - nearest_1.g - nearest_1.h;

    if (a_phase >= n_levels) {
        a_phase = n_levels - 1;
    }

    if (a_phase < 0) {
        a_phase = 0;
    }

    if (b_phase >= n_levels) {
        b_phase = n_levels - 1;
    }

    if (b_phase < 0) {
        b_phase = 0;
    }

    if (c_phase >= n_levels) {
        c_phase = n_levels - 1;
    }

    if (c_phase < 0) {
        c_phase = 0;
    }

    // GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[a_phase]); // TODO
    GPIOPinWrite(GPIO_PORTK_BASE, 0xFF, svm_phase_levels_b[b_phase]);
    // GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[a_phase]); // TODO

    state_counter++;

    float scaling_factor = 1;
    int8_t ref_val_a = value.a * scaling_factor;
    int8_t ref_val_b = value.b * scaling_factor;
    int8_t ref_val_c = value.c * scaling_factor;

    int8_t a_9_cell = ((svm_phase_levels_a[a_phase] & A_POS9) ? 1 : 0) +
                      ((svm_phase_levels_a[a_phase] & A_NEG9) ? -1 : 0);
    int8_t a_3_cell = ((svm_phase_levels_a[a_phase] & A_POS3) ? 1 : 0) +
                      ((svm_phase_levels_a[a_phase] & A_NEG3) ? -1 : 0);
    int8_t a_1_cell = ((svm_phase_levels_a[a_phase] & A_POS1) ? 1 : 0) +
                      ((svm_phase_levels_a[a_phase] & A_NEG1) ? -1 : 0);

    int8_t b_9_cell = ((svm_phase_levels_b[b_phase] & B_POS9) ? 1 : 0) +
                      ((svm_phase_levels_b[b_phase] & B_NEG9) ? -1 : 0);
    int8_t b_3_cell = ((svm_phase_levels_b[b_phase] & B_POS3) ? 1 : 0) +
                      ((svm_phase_levels_b[b_phase] & B_NEG3) ? -1 : 0);
    int8_t b_1_cell = ((svm_phase_levels_b[b_phase] & B_POS1) ? 1 : 0) +
                      ((svm_phase_levels_b[b_phase] & B_NEG1) ? -1 : 0);

    int8_t c_9_cell = ((svm_phase_levels_c[c_phase] & C_POS9) ? 1 : 0) +
                      ((svm_phase_levels_c[c_phase] & C_NEG9) ? -1 : 0);
    int8_t c_3_cell = ((svm_phase_levels_c[c_phase] & C_POS3) ? 1 : 0) +
                      ((svm_phase_levels_c[c_phase] & C_NEG3) ? -1 : 0);
    int8_t c_1_cell = ((svm_phase_levels_c[c_phase] & C_POS1) ? 1 : 0) +
                      ((svm_phase_levels_c[c_phase] & C_NEG1) ? -1 : 0);

    int8_t buffer[20] = {65,
                         97,
                         (int8_t)((a_phase - sizeof(svm_phase_levels_a) / 2) -
                                  (b_phase - sizeof(svm_phase_levels_b) / 2)),
                         (int8_t)((b_phase - sizeof(svm_phase_levels_b) / 2) -
                                  (c_phase - sizeof(svm_phase_levels_c) / 2)),
                         (int8_t)((c_phase - sizeof(svm_phase_levels_c) / 2) -
                                  (a_phase - sizeof(svm_phase_levels_a) / 2)),
                         ref_val_a,
                         ref_val_b,
                         ref_val_c,
                         a_9_cell,
                         a_3_cell,
                         a_1_cell,
                         b_9_cell,
                         b_3_cell,
                         b_1_cell,
                         c_9_cell,
                         c_3_cell,
                         c_1_cell};
    UART_write(uart, buffer, sizeof(buffer));
}
