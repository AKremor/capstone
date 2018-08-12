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

void svm_timer_callback(Timer_Handle handle);

volatile uint64_t state_counter = 0;

enum hb_pin {
    A_POS3 = 0x01,  // PL0
    A_NEG3 = 0x02,  // PL1
    A_OFF3 = 0x00,  // N/A
    A_POS9 = 0x04,  // PL2
    A_NEG9 = 0x08,  // PL3
    A_OFF9 = 0x00,  // N/A
    B_POS3 = 0x01,  // PK0
    B_NEG3 = 0x02,  // PK1
    B_POS9 = 0x04,  // PK2
    B_NEG9 = 0x08,  // PK3
    B_OFF9 = 0x00,  // N/A
    B_OFF3 = 0x00,  // N/A
    C_POS3 = 0x10,  // PK4
    C_NEG3 = 0x20,  // PK5
    C_POS9 = 0x40,  // PK6
    C_NEG9 = 0x80,  // PK7
    C_OFF9 = 0x00,  // N/A
    C_OFF3 = 0x00,  // N/A
};

/* 3 phase 3 level
uint8_t svm_phase_levels_a[] = {A_NEG3, 0x00, A_POS3};
uint8_t svm_phase_levels_b[] = {B_NEG3, 0x00, B_POS3};
uint8_t svm_phase_levels_c[] = {C_NEG3, 0x00, C_POS3};
*/

uint8_t svm_phase_levels_a[] = {
    A_NEG9 | A_NEG3, A_NEG9 | A_OFF3, A_NEG9 | A_POS3,
    A_OFF9 | A_NEG3, A_OFF9 | A_OFF3, A_OFF9 | A_POS3,
    A_POS9 | A_NEG3, A_POS9 | A_OFF3, A_POS9 | A_POS3};
uint8_t svm_phase_levels_b[] = {
    B_NEG9 | B_NEG3, B_NEG9 | B_OFF3, B_NEG9 | B_POS3,
    B_OFF9 | B_NEG3, B_OFF9 | B_OFF3, B_OFF9 | B_POS3,
    B_POS9 | B_NEG3, B_POS9 | B_OFF3, B_POS9 | B_POS3};
uint8_t svm_phase_levels_c[] = {
    C_NEG9 | C_NEG3, C_NEG9 | C_OFF3, C_NEG9 | C_POS3,
    C_OFF9 | C_NEG3, C_OFF9 | C_OFF3, C_OFF9 | C_POS3,
    C_POS9 | C_NEG3, C_POS9 | C_OFF3, C_POS9 | C_POS3};

void init_board() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,
                          GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    GPIOPinTypeGPIOOutput(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
};

Timer_Handle system_timer;
UART_Handle uart;

void uartcallback(UART_Handle handle, void *buf, size_t count) {}

void init_uart() {
    UART_init();

    UART_Params uart_params;
    UART_Params_init(&uart_params);
    uart_params.readEcho = UART_ECHO_OFF;
    uart_params.readDataMode = UART_DATA_BINARY;
    uart_params.writeDataMode = UART_DATA_BINARY;
    uart_params.baudRate = 921600;
    uart_params.readMode = UART_MODE_CALLBACK;
    uart_params.writeMode = UART_MODE_CALLBACK;
    uart_params.writeCallback = uartcallback;
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
    start_chopper();

    init_uart();

    init_board();

    init_timers();

    init_adc();

    uint32_t reading = 0;
    read_adc(&reading);
    int abc = 123;
}

void svm_timer_callback(Timer_Handle handle) {
    abc_quantity value = SineWave::getValueAbc(state_counter);

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

    int32_t a_phase = k;
    int32_t b_phase = k - nearest_1.g;
    int32_t c_phase = k - nearest_1.g - nearest_1.h;

    GPIOPinWrite(GPIO_PORTL_BASE, 0xFF, svm_phase_levels_a[a_phase]);
    GPIOPinWrite(GPIO_PORTK_BASE, 0xFF,
                 svm_phase_levels_b[b_phase] | svm_phase_levels_c[c_phase]);

    state_counter++;

    /*
    int8_t ref_val_mv = value.b * 10;

*/

    int8_t ref_val_mv = (value.b - value.c + value.c) * 10;
    int8_t buffer[7] = {0xCA,
                        0xFE,
                        (int8_t)a_phase - sizeof(svm_phase_levels_a) / 2,
                        (int8_t)(b_phase - sizeof(svm_phase_levels_b) / 2) -
                            (c_phase - sizeof(svm_phase_levels_c) / 2),
                        (int8_t)c_phase - sizeof(svm_phase_levels_c) / 2,
                        ref_val_mv,
                        0};

    UART_write(uart, buffer, 7);
}
