#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

extern void mainThread(void *arg0);

uint32_t systemClock;

int main(void) {
    systemClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                          SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                         120000000);
    mainThread(0);

    return (0);
}
