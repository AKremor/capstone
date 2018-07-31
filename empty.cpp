#include <src/mpc/mpc.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>
#include <ti/sysbios/knl/Task.h>
#include <unistd.h>
#include "Board.h"
#include "arm_math.h"

void initSineWaveOutput();

extern "C" {

void timerCallback(Timer_Handle handle);
void chopper_timer_callback(Timer_Handle handle);

#define ADCBUFFERSIZE (5)

uint16_t sampleBufferOne[ADCBUFFERSIZE];
uint16_t sampleBufferTwo[ADCBUFFERSIZE];
float voltageBuffer[ADCBUFFERSIZE];
uint32_t buffersCompletedCounter = 0;

/* This function converts a differential buffer to signed microvolts*/
int_fast16_t convertAdjustedDifferential(ADCBuf_Handle handle,
                                         uint32_t adcChannel,
                                         void *adjustedSampleBuffer,
                                         float outputDifferentialBuffer[],
                                         uint_fast16_t sampleCount) {
    uint32_t i;
    uint16_t *adjustedRawSampleBuf = (uint16_t *)adjustedSampleBuffer;

    float refVoltage = 3.3f;

    /* Converts the ADC result (14-bit) to a float with respect to refVoltage */
    for (i = 0; i < sampleCount; i++) {
        if (adjustedRawSampleBuf[i] == 0x800) {
            outputDifferentialBuffer[i] = 0;
        } else {
            outputDifferentialBuffer[i] =
                (refVoltage * (adjustedRawSampleBuf[i] - 0x800)) / 0x800;
        }
    }

    return ADCBuf_STATUS_SUCCESS;
}

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
                    void *completedADCBuffer, uint32_t completedChannel) {
    /* Adjust raw adc values and convert them to microvolts */
    convertAdjustedDifferential(handle, completedChannel, completedADCBuffer,
                                voltageBuffer, ADCBUFFERSIZE);
}

void *mainThread(void *arg0) {
    GPIO_init();
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

    Timer_Handle timer_chopper;
    Timer_Params chopper_params;

    Timer_Params_init(&chopper_params);
    chopper_params.period = 2 * 20000;
    chopper_params.periodUnits = Timer_PERIOD_HZ;
    chopper_params.timerMode = Timer_CONTINUOUS_CALLBACK;
    chopper_params.timerCallback = chopper_timer_callback;

    timer_chopper = Timer_open(Board_TIMER1, &chopper_params);

    if (timer_chopper == NULL) {
        while (1)
            ;
    }

    if (Timer_start(timer_chopper) == Timer_STATUS_ERROR) {
        while (1)
            ;
    }

    // Configure ADC
    ADCBuf_Handle adcBuf;
    ADCBuf_Params adcBufParams;
    ADCBuf_Conversion continuousConversion;
    ADCBuf_init();

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcBufParams.samplingFrequency = 200;
    adcBuf = ADCBuf_open(Board_ADCBUF0, &adcBufParams);

    /* Configure the conversion struct */
    continuousConversion.arg = NULL;
    continuousConversion.adcChannel = Board_ADCBUF0CHANNEL3;
    continuousConversion.sampleBuffer = sampleBufferOne;
    continuousConversion.sampleBufferTwo = sampleBufferTwo;
    continuousConversion.samplesRequestedCount = ADCBUFFERSIZE;

    if (!adcBuf) {
        /* AdcBuf did not open correctly. */
        while (1)
            ;
    }

    /* Start converting. */
    // if (ADCBuf_convert(adcBuf, &continuousConversion, 1) !=
    //    ADCBuf_STATUS_SUCCESS) {
    //    /* Did not start conversion process correctly. */
    //    while (1)
    //        ;
    //}
}

volatile uint64_t state_counter = 0;

// PL0 9VL POS
// PL1 9VR NEG
// PL2 3VL POS
// PL3 3VR NEG

#define POS9 0x01
#define NEG9 0x02
#define POS3 0x04
#define NEG3 0x08
#define OFF9 0x00
#define OFF3 0x00

uint8_t states[] = {
    NEG9 | NEG3, NEG9 | OFF3, NEG9 | POS3, OFF9 | NEG3, OFF9 | OFF3,
    OFF9 | POS3, POS9 | NEG3, POS9 | OFF3, POS9 | POS3,

    POS9 | POS3, POS9 | OFF3, POS9 | NEG3, OFF9 | POS3, OFF9 | OFF3,
    OFF9 | NEG3, NEG9 | POS3, NEG9 | OFF3, NEG9 | NEG3

};

int findNearestStateIndex(uint64_t state_counter) {
    uint8_t middle_offset = sizeof(states) / 2;

    double sine_val = (sizeof(states) / 2) * sin(1.0 * state_counter / 20);
    int level = floor(sine_val + 0.5) + middle_offset;
    return level;
}
void timerCallback(Timer_Handle myHandle) {
    // int nearest_state_index = findNearestStateIndex(state_counter);
    GPIOPinWrite(GPIO_PORTL_BASE, 0b1111,
                 states[state_counter % sizeof(states)]);
    state_counter++;
}

volatile bool chopper_flag = true;
void chopper_timer_callback(Timer_Handle handle) {
    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                 GPIO_PIN_0 << chopper_flag);
    chopper_flag = !chopper_flag;
}
}
