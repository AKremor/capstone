#include <source/hal/adc.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

void clearSpiFifo();
void adcSelect();
void adcDeselect();
uint8_t adcReadRegister(uint8_t command_value, uint8_t* value);
uint8_t adcWriteRegister(uint8_t command_value, uint8_t value);
uint8_t adcEnableAutoRst();

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

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6);

    MAP_GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    MAP_GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    MAP_GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    MAP_GPIOPinConfigure(GPIO_PD3_SSI2CLK);

    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE,
                       GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    MAP_SSIConfigSetExpClk(SSI2_BASE, 120E6, SSI_FRF_MOTO_MODE_1,
                           SSI_MODE_MASTER, (120E6 / 120), 8);

    MAP_SSIEnable(SSI2_BASE);

    clearSpiFifo();

    adcWriteFeatureSelect();

    // adcEnableAutoRst();
    // adcWriteFeatureSelect();
}

void adcReadAutoSeqEn() {
    uint8_t register_value;
    adcReadRegister(0x01, &register_value);
}

void adcReadChannelPowerDown() {
    uint8_t register_value;
    adcReadRegister(0x02, &register_value);
}

void adcWriteFeatureSelect() {
    adcWriteRegister(0x03, 0x03);
    uint8_t register_value;
    adcReadRegister(0x03, &register_value);
    ASSERT(0x03 == register_value);
}

uint8_t adcReadRegister(uint8_t command_value, uint8_t* value) {
    clearSpiFifo();

    uint32_t cmd_read[3] = {0};
    uint8_t read_command = command_value << 1 | 0;
    adcSelect();
    MAP_SSIDataPut(SSI2_BASE, read_command);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    while (MAP_SSIBusy(SSI2_BASE))
        ;
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[0]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[1]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[2]);
    *value = cmd_read[2];
    adcDeselect();

    return 0;
}

uint8_t adcSetChannel(AdcChannelId channel_id) {
    clearSpiFifo();

    uint32_t cmd_read[3] = {0};
    uint8_t read_command = 0xA0;
    adcSelect();
    MAP_SSIDataPut(SSI2_BASE, channel_id);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    while (MAP_SSIBusy(SSI2_BASE))
        ;

    clearSpiFifo();
    adcDeselect();

    return 0;
}

uint8_t adcEnableAutoRst() {
    clearSpiFifo();

    uint32_t cmd_read[3] = {0};
    uint8_t read_command = 0xA0;
    adcSelect();
    MAP_SSIDataPut(SSI2_BASE, read_command);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    while (MAP_SSIBusy(SSI2_BASE))
        ;
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[0]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[1]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[2]);

    adcDeselect();

    return 0;
}

uint8_t adcWriteRegister(uint8_t command_value, uint8_t value) {
    clearSpiFifo();

    uint32_t cmd_read[3] = {0};
    uint8_t read_command = command_value << 1 | 1;
    adcSelect();
    MAP_SSIDataPut(SSI2_BASE, read_command);
    MAP_SSIDataPut(SSI2_BASE, value);
    MAP_SSIDataPut(SSI2_BASE, 0x00);
    while (MAP_SSIBusy(SSI2_BASE))
        ;
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[0]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[1]);
    MAP_SSIDataGet(SSI2_BASE, &cmd_read[2]);

    adcDeselect();

    return 0;
}

void adcReadChannels(float* channel_data) {
    uint32_t data[4];
    uint8_t n_channels = 3;
    AdcChannelId channels[] = {MAN_CH_0, MAN_CH_1, MAN_CH_2};

    clearSpiFifo();

    // Write and read is offset by one

    for (int i = 0; i < n_channels; i++) {
        clearSpiFifo();
        adcSelect();

        MAP_SSIDataPut(SSI2_BASE, channels[i]);
        MAP_SSIDataPut(SSI2_BASE, 0x00);
        MAP_SSIDataPut(SSI2_BASE, 0x00);
        MAP_SSIDataPut(SSI2_BASE, 0x00);
        while (MAP_SSIBusy(SSI2_BASE))
            ;

        MAP_SSIDataGet(SSI2_BASE, &data[0]);
        MAP_SSIDataGet(SSI2_BASE, &data[1]);
        MAP_SSIDataGet(SSI2_BASE, &data[2]);
        MAP_SSIDataGet(SSI2_BASE, &data[3]);
        adcDeselect();

        volatile uint32_t digital = data[2] << 8 | data[3];

        // Assuming conversion is in different bit location
        volatile uint16_t conversion_shifted = (digital >> 2) & 0xFFFF;
        volatile float converted_shifted =
            conversion_shifted * 1250 * 1e-6 - 10.24;

        // Because write/reads are offset by one
        uint8_t write_index;
        if (i - 1 < 0) {
            write_index = n_channels - 1;
        } else {
            write_index = i - 1;
        }
        channel_data[write_index] = converted_shifted;
    }
}

void adcSelect() {
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0x00);
    SysCtlDelay(10);
}

void adcDeselect() {
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_PIN_6);
    SysCtlDelay(10);
}

void clearSpiFifo() {
    uint32_t dummyRead[1];
    while (MAP_SSIDataGetNonBlocking(SSI2_BASE, &dummyRead[0]))
        ;
}
