#include <Board.h>
#include <src/chopper/chopper.h>
#include <src/system_config.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/Timer.h>

extern "C" {

void timerCallback(Timer_Handle handle);

void *mainThread(void *arg0) {
    start_chopper();

    while (true)
        ;
    Timer_init();

    /* Configure Port N pin 1 as output. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)))
        ;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)))
        ;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)))
        ;

    GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = 9 * 1000;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (timer0 == NULL) {
        while (1)
            ;
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1)
            ;
    }

    return 0;
}

volatile uint64_t state_counter = 0;

uint8_t states[] = {
    NEG9 | NEG3, NEG9 | OFF3, NEG9 | POS3, OFF9 | NEG3, OFF9 | OFF3,
    OFF9 | POS3, POS9 | NEG3, POS9 | OFF3, POS9 | POS3,

    POS9 | POS3, POS9 | OFF3, POS9 | NEG3, OFF9 | POS3, OFF9 | OFF3,
    OFF9 | NEG3, NEG9 | POS3, NEG9 | OFF3, NEG9 | NEG3

};

void timerCallback(Timer_Handle myHandle) {
    GPIOPinWrite(GPIO_PORTL_BASE, 0b1111,
                 states[state_counter % sizeof(states)]);
    state_counter++;
}
}
