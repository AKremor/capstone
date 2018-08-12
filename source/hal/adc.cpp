#include <source/hal/MSP_EXP432E401Y.h>
#include <source/hal/adc.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

void init_adc() {
    /* Enable the clock to GPIO Port E and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))) {
    }

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);

    /* Enable the clock to ADC-0 and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))) {
    }

    /* Configure Sequencer 2 to sample the differential analog channels. The
     * end of conversion and interrupt generation is set for differential
     * pair 1 */
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH0 | ADC_CTL_D);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_CH1 | ADC_CTL_D);
    ADCSequenceStepConfigure(
        ADC0_BASE, 2, 3, ADC_CTL_CH4 | ADC_CTL_D | ADC_CTL_IE | ADC_CTL_END);

    /* Enable sample sequence 2 with a processor signal trigger.  Sequencer 2
     * will do a single sample when the processor sends a signal to start the
     * conversion */
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 2);

    /* Since sample sequence 2 is now configured, it must be enabled. */
    ADCSequenceEnable(ADC0_BASE, 2);

    /* Clear the interrupt status flag.  This is done to make sure the
     * interrupt flag is cleared before we sample. */
    ADCIntClear(ADC0_BASE, 2);
}

float32_t convertAdjustedDifferential(int32_t raw_sample) {
    float refVoltage = 3.3f;

    if (raw_sample == 0x800) {
        return 0;
    } else {
        return (refVoltage * (raw_sample - 0x800)) / 0x800;
    }
}

void read_adc(uint32_t *reading) {
    uint32_t getADCValue[3];
    /* Sample the channels forever.  Display the value on the console. */
    while (1) {
        /* Trigger the ADC conversion. */
        ADCProcessorTrigger(ADC0_BASE, 2);

        /* Wait for conversion to be completed. */
        while (!ADCIntStatus(ADC0_BASE, 2, false)) {
        }

        /* Clear the ADC interrupt flag. */
        ADCIntClear(ADC0_BASE, 2);

        /* Read ADC Value. */
        ADCSequenceDataGet(ADC0_BASE, 2, getADCValue);
        float32_t samples[3];
        for (int i = 0; i < 3; i++) {
            samples[i] = convertAdjustedDifferential(getADCValue[i]);
        }
        int abc = 123;
    }
}
