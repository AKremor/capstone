#include <src/chopper/chopper.h>
#include <src/hal/Board.h>
#include <src/reference_signal/sine_wave.h>
#include <src/system_config.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>
#include <xdc/runtime/System.h>
#include "arm_math.h"

void svm_timer_callback(Timer_Handle handle);

volatile uint64_t state_counter = 0;

// Pins
// A phase

enum hb_pin {
    A_POS3 = 0x01,  // PL0
    A_NEG3 = 0x02,  // PL1
    A_POS9 = 0x04,  // PL2
    A_NEG9 = 0x08,  // PL3
    B_POS3 = 0x01,  // PK0
    B_NEG3 = 0x02,  // PK1
    B_POS9 = 0x04,  // PK2
    B_NEG9 = 0x08,  // PK3
    C_POS3 = 0x10,  // PK4
    C_NEG3 = 0x20,  // PK5
    C_POS9 = 0x40,  // PK6
    C_NEG9 = 0x80   // PK7
};

uint8_t svm_phase_levels_a[] = {A_NEG3, 0x00, A_POS3};
uint8_t svm_phase_levels_b[] = {B_NEG3, 0x00, B_POS3};
uint8_t svm_phase_levels_c[] = {C_NEG3, 0x00, C_POS3};

void init_board() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,
                          GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE,
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
};

void *mainThread(void *arg0) {
    start_chopper();

    init_board();

    Timer_init();

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = svm_frequency_hz;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = svm_timer_callback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        System_abort("SVM timer did not start");
    }

    return 0;
}

void svm_timer_callback(Timer_Handle handle) {
    float32_t Vdc = 1;
    abc_quantity value = SineWave::getValueAbc(state_counter);

    gh_quantity hex_value;
    hex_value.g = 1 / (3 * Vdc) * (2 * value.a - value.b - value.c);
    hex_value.h = 1 / (3 * Vdc) * (-1 * value.a + 2 * value.b - value.c);

    gh_quantity Vul = {ceilf(hex_value.g), floorf(hex_value.h)};
    gh_quantity Vlu = {floorf(hex_value.g), ceilf(hex_value.h)};
    gh_quantity Vuu = {ceilf(hex_value.g), ceilf(hex_value.h)};
    gh_quantity Vll = {floorf(hex_value.g), floorf(hex_value.h)};

    gh_quantity nearest_1 = Vul;

    // Now we need to find an available voltage state.
    // This is a very rudimentary implementation

    int32_t k = 0;
    bool constraints_satisfied = false;
    int32_t g = nearest_1.g;
    int32_t h = nearest_1.h;
    int32_t n = 3;  // TODO
    while (!constraints_satisfied) {
        if (k >= 0 && k - g >= 0 && k - g - h >= 0 && k <= n - 1 &&
            k - g <= n - 1 && k - g - h <= n - 1) {
            constraints_satisfied = true;
            break;
        }
        if (k >= n - 1) {
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
}
