#include <source/hal/adc.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

constexpr uint8_t n_channels = 3;
constexpr uint8_t sequencer_2 = 2;

void init_adc() {
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))) {
    }

    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);  // AIN3
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);  // AIN2
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);  // AIN1

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))) {
    }

    // Deals with errata ADC#14, otherwise first two samples are invalid
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))) {
    }

    // Read in order of AIN increasing
    MAP_ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 0, ADC_CTL_CH3);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 1, ADC_CTL_CH2);
    MAP_ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 2,
                                 ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    MAP_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_TIMER, 2);

    MAP_ADCSequenceEnable(ADC0_BASE, 2);

    MAP_ADCIntClear(ADC0_BASE, 2);
    MAP_ADCIntEnable(ADC0_BASE, 2);
    MAP_IntEnable(INT_ADC0SS2);
}

float32_t convertAdjustedSingle(int32_t raw_sample) {
    float refVoltage = 3.3f;
    return refVoltage * (((float)raw_sample) / 0x1000);
}

void read_adc(float32_t* reading) {

}
