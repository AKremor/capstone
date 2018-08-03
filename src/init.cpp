#include <src/chopper/chopper.h>
#include <src/hal/Board.h>
#include <src/reference_signal/sine_wave.h>
#include <src/system_config.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>
#include <xdc/runtime/System.h>
FILE *fp;
void timerCallback(Timer_Handle handle);

// PL0 9VL POS
// PL1 9VR NEG
// PL2 3VL POS
// PL3 3VR NEG

// A phase - L
// B phase - K
// C phase - C

// Port L
#define A_POS9 0x01
#define A_NEG9 0x02
#define A_POS3 0x04
#define A_NEG3 0x08
#define A_OFF9 0x00
#define A_OFF3 0x00

#define B_POS9 0x10
#define B_NEG9 0x40
#define B_POS3 0x50
#define B_NEG3 0x60
#define B_OFF9 0x00
#define B_OFF3 0x00

#define C_POS9 0x10
#define C_NEG9 0x40
#define C_POS3 0x50
#define C_NEG3 0x60
#define C_OFF9 0x00
#define C_OFF3 0x00

uint8_t states[] = {
    A_NEG9 | A_NEG3, A_NEG9 | A_OFF3, A_NEG9 | A_POS3, A_OFF9 | A_NEG3,
    A_OFF9 | A_OFF3, A_OFF9 | A_POS3, A_POS9 | A_NEG3, A_POS9 | A_OFF3,
    A_POS9 | A_POS3,

    A_POS9 | A_POS3, A_POS9 | A_OFF3, A_POS9 | A_NEG3, A_OFF9 | A_POS3,
    A_OFF9 | A_OFF3, A_OFF9 | A_NEG3, A_NEG9 | A_POS3, A_NEG9 | A_OFF3,
    A_NEG9 | A_NEG3

};

Timer_Handle timer1;

void *mainThread(void *arg0) {
    start_chopper();

    Timer_init();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)))
        ;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,
                          GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE,
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = sizeof(states) * svm_frequency_hz;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        System_abort("SVM timer did not start");
    }

    Timer_Params params1;

    Timer_Params_init(&params1);
    params1.timerMode = Timer_FREE_RUNNING;

    timer1 = Timer_open(Board_TIMER1, &params1);

    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
        System_abort("SVM timer did not start");
    }

    return 0;
}

volatile uint64_t state_counter = 0;

// TODO(akremor): Sizing and naming
static uint32_t port_buffer[15] = {0};

void setPinsBuffered(uint32_t ui32Port, uint8_t ui8Pins, uint8_t ui8Val) {
    // Takes in pins that can be | to set the value to val
    // Some freaky memory shifting to resolve the base into a 0-indexed sequence
    // GPIO_PORTA_BASE                 ((uint32_t)0x40058000)

    uint8_t port_buffer_index = (ui32Port >> 3) - 88;

    if (ui8Val) {
        port_buffer[port_buffer_index] |= ui8Pins;
    } else {
        port_buffer[port_buffer_index] &= ~(ui8Pins);
    }
}

void applyPortSetting(uint32_t ui32Port) {
    uint8_t port_buffer_index = (ui32Port >> 3) - 88;
    GPIOPinWrite(ui32Port, 0xFF, port_buffer[port_buffer_index]);
}

void timerCallback(Timer_Handle myHandle) {
    float32_t Vdc = 1;
    float32_t time = Timer_getCount(timer1);
    abc_quantity value = SineWave::getValueAbc(state_counter);

    // TODO(akremor): Need to ensure the output magnitude is scaled
    // appropriately

    gh_quantity hex_value;
    hex_value.g = 1 / (3 * Vdc) * (2 * value.a - value.b - value.c);
    hex_value.h = 1 / (3 * Vdc) * (-1 * value.a + 2 * value.b - value.c);

    gh_quantity Vul = {ceil(hex_value.g), floor(hex_value.h)};
    gh_quantity Vlu = {floor(hex_value.g), ceil(hex_value.h)};
    gh_quantity Vuu = {ceil(hex_value.g), ceil(hex_value.h)};
    gh_quantity Vll = {floor(hex_value.g), floor(hex_value.h)};

    gh_quantity nearest_1 = Vul;
    gh_quantity nearest_2 = Vlu;
    gh_quantity nearest_3;

    if (copysignf(1.0, hex_value.g + hex_value.h - Vul.g - Vul.h) == 1) {
        nearest_3 = Vuu;
    } else {
        nearest_3 = Vll;
    }

    // Apply gh transform

    // Now we need to find an available voltage state

    bool all_satisfied = false;
    int32_t k = -4;
    int32_t abs_bound = 4;
    while (!all_satisfied) {
        if ((-1 * abs_bound <= k && k <= abs_bound) &&
            (-1 * abs_bound <= k - nearest_1.g &&
             k - nearest_1.g <= abs_bound) &&
            (-1 * abs_bound <= k - nearest_1.g - nearest_1.h &&
             k - nearest_1.g - nearest_1.h <= abs_bound)) {
            all_satisfied = true;
        } else {
            k++;
        }
    }

    int32_t a_phase = k;
    int32_t b_phase = k - nearest_1.g;
    int32_t c_phase = k - nearest_1.g - nearest_1.h;

    setPinsBuffered(GPIO_PORTC_BASE, 0xFF, false);
    setPinsBuffered(GPIO_PORTL_BASE, 0xFF, false);
    setPinsBuffered(GPIO_PORTK_BASE, 0xFF, false);

    switch (a_phase) {
        case -4:
            setPinsBuffered(GPIO_PORTL_BASE, A_NEG9 | A_NEG3, true);
            break;
        case -3:
            setPinsBuffered(GPIO_PORTL_BASE, A_NEG9 | A_OFF3, true);
            break;
        case -2:
            setPinsBuffered(GPIO_PORTL_BASE, A_NEG9 | A_POS3, true);
            break;
        case -1:
            setPinsBuffered(GPIO_PORTL_BASE, A_OFF9 | A_POS3, true);
            break;
        case 0:
            setPinsBuffered(GPIO_PORTL_BASE, A_OFF9 | A_OFF3, true);
            break;
        case 1:
            setPinsBuffered(GPIO_PORTL_BASE, A_OFF9 | A_POS3, true);
            break;
        case 2:
            setPinsBuffered(GPIO_PORTL_BASE, A_POS9 | A_NEG3, true);
            break;
        case 3:
            setPinsBuffered(GPIO_PORTL_BASE, A_POS9 | A_OFF3, true);
            break;
        case 4:
            setPinsBuffered(GPIO_PORTL_BASE, A_POS9 | A_POS3, true);
            break;
    }

    switch (b_phase) {
        case -4:
            setPinsBuffered(GPIO_PORTK_BASE, B_NEG9 | B_NEG3, true);
            break;
        case -3:
            setPinsBuffered(GPIO_PORTK_BASE, B_NEG9 | B_OFF3, true);
            break;
        case -2:
            setPinsBuffered(GPIO_PORTK_BASE, B_NEG9 | B_POS3, true);
            break;
        case -1:
            setPinsBuffered(GPIO_PORTK_BASE, B_OFF9 | B_POS3, true);
            break;
        case 0:
            setPinsBuffered(GPIO_PORTK_BASE, B_OFF9 | B_OFF3, true);
            break;
        case 1:
            setPinsBuffered(GPIO_PORTK_BASE, B_OFF9 | B_POS3, true);
            break;
        case 2:
            setPinsBuffered(GPIO_PORTK_BASE, B_POS9 | B_NEG3, true);
            break;
        case 3:
            setPinsBuffered(GPIO_PORTK_BASE, B_POS9 | B_OFF3, true);
            break;
        case 4:
            setPinsBuffered(GPIO_PORTK_BASE, B_POS9 | B_POS3, true);
            break;
    }

    switch (c_phase) {
        case -4:
            setPinsBuffered(GPIO_PORTC_BASE, C_NEG9 | C_NEG3, true);
            break;
        case -3:
            setPinsBuffered(GPIO_PORTC_BASE, C_NEG9 | C_OFF3, true);
            break;
        case -2:
            setPinsBuffered(GPIO_PORTC_BASE, C_NEG9 | C_POS3, true);
            break;
        case -1:
            setPinsBuffered(GPIO_PORTC_BASE, C_OFF9 | C_POS3, true);
            break;
        case 0:
            setPinsBuffered(GPIO_PORTC_BASE, C_OFF9 | C_OFF3, true);
            break;
        case 1:
            setPinsBuffered(GPIO_PORTC_BASE, C_OFF9 | C_POS3, true);
            break;
        case 2:
            setPinsBuffered(GPIO_PORTC_BASE, C_POS9 | C_NEG3, true);
            break;
        case 3:
            setPinsBuffered(GPIO_PORTC_BASE, C_POS9 | C_OFF3, true);
            break;
        case 4:
            setPinsBuffered(GPIO_PORTC_BASE, C_POS9 | C_POS3, true);
            break;
    }

    // setPinsBuffered(GPIO_PORTL_BASE, states[state_counter % sizeof(states)],
    //                true);
    applyPortSetting(GPIO_PORTC_BASE);
    applyPortSetting(GPIO_PORTL_BASE);
    applyPortSetting(GPIO_PORTK_BASE);
    // fp = fopen("svm.csv", "w");
    System_printf("%d,%d,%d\n", a_phase, b_phase, c_phase);
    System_flush();
    // fclose(fp);
    state_counter++;
}
