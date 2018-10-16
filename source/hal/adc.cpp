#include <source/hal/adc.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

void clearSpiFifo();
void adcSelect();
void adcDeselect();

void init_adc() {
    // The SSI1 peripheral must be disabled, reset and re enabled for use
    // Wait till the Peripheral ready is not asserted
    MAP_SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI2);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_SSI2);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2))) {
    }

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))) {
    }

    MAP_GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    MAP_GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    MAP_GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    MAP_GPIOPinConfigure(GPIO_PD3_SSI2CLK);

    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE,
                       GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    MAP_SSIConfigSetExpClk(SSI2_BASE, 120E6, SSI_FRF_MOTO_MODE_0,
                           SSI_MODE_MASTER, (120E6 / 120), 16);

    SSI2->CR1 |= SSI_CR1_HSCLKEN;
    MAP_SSIEnable(SSI2_BASE);

    clearSpiFifo();

    // TODO Can I always assert CS?
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);

    // Set up for auto channel scan mode
    // AUTO RST command
    adcSelect();
    MAP_SSIDataPut(SSI2_BASE, 0xA000);
    adcDeselect();
}

void adcReadChannels(float* channel_data) {
    uint32_t junk_data[1];
    uint32_t data[1];
    uint8_t n_channels = 8;

    for (int i = 0; i < n_channels; i++) {
        adcSelect();
        MAP_SSIDataPut(SSI2_BASE, 0x00);
        MAP_SSIDataGet(SSI2_BASE, junk_data);
        MAP_SSIDataPut(SSI2_BASE, 0x00);
        MAP_SSIDataGet(SSI2_BASE, data);
        adcDeselect();

        // Now do the conversion... TODO
        channel_data[i] = data[0];
    }
}

void adcSelect() {
    SysCtlDelay(120E6 * 0.001);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0x00);
    SysCtlDelay(120E6 * 0.001);
}

void adcDeselect() {
    SysCtlDelay(120E6 * 0.001);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_PIN_6);
    SysCtlDelay(120E6 * 0.001);
}

void clearSpiFifo() {
    uint32_t dummyRead[1];
    while (MAP_SSIDataGetNonBlocking(SSI2_BASE, &dummyRead[0])) {
    }
}
