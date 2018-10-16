#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>

extern void mainThread(void *arg0);

uint32_t system_clock_hz;

int main(void) {
    system_clock_hz = MAP_SysCtlClockFreqSet(
        SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    mainThread(0);

    return (0);
}
