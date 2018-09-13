#include <source/hal/adc.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include "arm_math.h"

constexpr uint8_t n_channels = 3;
constexpr uint8_t sequencer_2 = 2;

uint32_t getADCValue[n_channels];

void ADC0SS2_IRQHandler(void) {
    SystemState* state = SystemState::get();
    uint32_t getIntStatus;

    /* Get the interrupt status from the ADC */
    getIntStatus = ADCIntStatus(ADC0_BASE, sequencer_2, true);

    /* If the interrupt status for Sequencer-2 is set the
     * clear the status and read the data */
    if (getIntStatus == 0x4) {
        /* Clear the ADC interrupt flag. */
        ADCIntClear(ADC0_BASE, sequencer_2);

        /* Read ADC Value. */
        ADCSequenceDataGet(ADC0_BASE, sequencer_2, getADCValue);
        float32_t reading[n_channels];
        for (int i = 0; i < n_channels; i++) {
            reading[i] = convertAdjustedSingle(getADCValue[i]);
        }

        // Bias current readings appropriately
        abc_quantity quantity_current = {2 * (reading[0] - 1.55f),
                                         2 * (reading[1] - 1.55f),
                                         2 * (reading[2] - 1.55f)};
        state->load_line_current.set_abc(quantity_current);
    }
}

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
    ADCIntEnable(ADC0_BASE, sequencer_2);

    IntEnable(INT_ADC0SS2);

    /* Enable Timer-0 clock and configure the timer in periodic mode with
     * a frequency of 10 KHz. Enable the ADC trigger generation from the
     * timer-0. */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))) {
    }

    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, adc_period);
    TimerADCEventSet(TIMER1_BASE, TIMER_ADC_TIMEOUT_A);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
    TimerEnable(TIMER1_BASE, TIMER_A);
}

float32_t convertAdjustedSingle(int32_t raw_sample) {
    float refVoltage = 3.3f;
    return refVoltage * (((float)raw_sample) / 0x1000);
}
