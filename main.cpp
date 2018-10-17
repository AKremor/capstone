#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/debug.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/devices/msp432e4/driverlib/inc/hw_nvic.h>
extern void mainThread(void *arg0);

void __error__(char *pcFilename, uint32_t ui32Line) { asm("bkpt\n\r"); }

uint32_t system_clock_hz;

int main(void) {
    SCnSCB->ACTLR |= NVIC_ACTLR_DISWBUF;

    system_clock_hz = MAP_SysCtlClockFreqSet(
        SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    mainThread(0);

    return (0);
}
