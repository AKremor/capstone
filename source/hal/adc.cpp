#include <source/hal/adc.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

constexpr uint8_t n_channels = 3;
constexpr uint8_t sequencer_2 = 2;

void init_adc() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))) {
    }

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);  // AIN3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);  // AIN2
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);  // AIN1

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))) {
    }

    // Deals with errata ADC#14, otherwise first two samples are invalid
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))) {
    }

    // Read in order of AIN increasing
    ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 0, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 1, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, sequencer_2, 2,
                             ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    /* Enable sample sequence 2 with a processor signal trigger.  Sequencer 2
     * will do a single sample when the processor sends a signal to start the
     * conversion */
    ADCSequenceConfigure(ADC0_BASE, sequencer_2, ADC_TRIGGER_PROCESSOR, 2);

    /* Since sample sequence 2 is now configured, it must be enabled. */
    ADCSequenceEnable(ADC0_BASE, sequencer_2);

    /* Clear the interrupt status flag.  This is done to make sure the
     * interrupt flag is cleared before we sample. */
    ADCIntClear(ADC0_BASE, sequencer_2);
}

float32_t convertAdjustedSingle(int32_t raw_sample) {
    float refVoltage = 3.3f;
    return refVoltage * (((float)raw_sample) / 0x1000);
}

void read_adc(float32_t* reading) {
    uint32_t getADCValue[n_channels];

    ADCProcessorTrigger(ADC0_BASE, sequencer_2);

    // Wait for conversion complete interrupt
    while (!ADCIntStatus(ADC0_BASE, sequencer_2, false)) {
    }

    ADCIntClear(ADC0_BASE, sequencer_2);

    ADCSequenceDataGet(ADC0_BASE, sequencer_2, getADCValue);
    for (int i = 0; i < n_channels; i++) {
        reading[i] = convertAdjustedSingle(getADCValue[i]);
    }
}
