#include <src/chopper/chopper.h>
#include <src/hal/Board.h>
#include <src/reference_signal/sine_wave.h>
#include <src/system_config.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>
#include <xdc/runtime/System.h>

void timerCallback(Timer_Handle handle);

// PL0 9VL POS
// PL1 9VR NEG
// PL2 3VL POS
// PL3 3VR NEG

// Port L
#define POS9 0x01
#define NEG9 0x02
#define POS3 0x04
#define NEG3 0x08
#define OFF9 0x00
#define OFF3 0x00

uint8_t states[] = {
    NEG9 | NEG3, NEG9 | OFF3, NEG9 | POS3, OFF9 | NEG3, OFF9 | OFF3,
    OFF9 | POS3, POS9 | NEG3, POS9 | OFF3, POS9 | POS3,

    POS9 | POS3, POS9 | OFF3, POS9 | NEG3, OFF9 | POS3, OFF9 | OFF3,
    OFF9 | NEG3, NEG9 | POS3, NEG9 | OFF3, NEG9 | NEG3

};

void *mainThread(void *arg0) {
    start_chopper();

    Timer_init();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,
                          GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

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

    return 0;
}

volatile uint64_t state_counter = 0;

// TODO(akremor): Sizing and naming
static uint32_t port_buffer[15] = {0};

void setPinsBuffered(uint32_t ui32Port, uint8_t ui8Pins, uint8_t ui8Val) {
    // Takes in pins that can be | to set the value to val
    // Some freaky memory shifting to resolve the base into a 0-indexed sequence
    // GPIO_PORTA_BASE                 ((uint32_t)0x40058000)

    // TODO(akremor): Confirm this logic works
    uint8_t port_buffer_index = (ui32Port >> 3) - 88;

    if (ui8Val) {
        port_buffer[port_buffer_index] |= ui8Pins;
    } else {
        port_buffer[port_buffer_index] &= ~(ui8Pins);
    }
}

void applyPortSetting(uint32_t ui32Port) {
    // TODO(akremor): Confirm this logic works
    uint8_t port_buffer_index = (ui32Port >> 3) - 88;
    GPIOPinWrite(ui32Port, 0xFF, port_buffer[port_buffer_index]);
}

void timerCallback(Timer_Handle myHandle) {
    // GPIOPinWrite(GPIO_PORTL_BASE, 0b1111,
    //             states[state_counter % sizeof(states)]);

    setPinsBuffered(GPIO_PORTL_BASE, 0xFF, false);
    setPinsBuffered(GPIO_PORTL_BASE, states[state_counter % sizeof(states)],
                    true);
    applyPortSetting(GPIO_PORTL_BASE);
    state_counter++;
}
