#include <source/hal/MSP_EXP432E401Y.h>
#include <source/hal/adc.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/drivers/ADCBuf.h>

ADCBuf_Conversion continuousConversion[6];
uint16_t sampleBufferOne[6] = {NULL};
uint16_t sampleBufferTwo[6] = {NULL};
float outputBuffer[6] = {NULL};
ADCBuf_Handle adcBuf;

void init_adc() {
    ADCBuf_init();
    ADCBuf_Params adcBufParams;
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = NULL;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
    adcBufParams.samplingFrequency = 200;
    adcBuf = ADCBuf_open(MSP_EXP432E401Y_ADCBUF0, &adcBufParams);

    /* Configure the conversion struct for two channels on same sequencer */
    continuousConversion[0].arg = NULL;
    continuousConversion[0].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL0;
    continuousConversion[0].sampleBuffer = &sampleBufferOne[0];
    continuousConversion[0].sampleBufferTwo = NULL;
    continuousConversion[0].samplesRequestedCount = 1;

    continuousConversion[1].arg = NULL;
    continuousConversion[1].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL1;
    continuousConversion[1].sampleBuffer = &sampleBufferOne[1];
    continuousConversion[1].sampleBufferTwo = NULL;
    continuousConversion[1].samplesRequestedCount = 1;

    continuousConversion[2].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL2;
    continuousConversion[2].sampleBuffer = &sampleBufferOne[2];
    continuousConversion[2].samplesRequestedCount = 1;

    continuousConversion[3].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL3;
    continuousConversion[3].sampleBuffer = &sampleBufferOne[3];
    continuousConversion[3].samplesRequestedCount = 1;

    continuousConversion[4].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL4;
    continuousConversion[4].sampleBuffer = &sampleBufferOne[4];
    continuousConversion[4].samplesRequestedCount = 1;

    continuousConversion[5].adcChannel = MSP_EXP432E401Y_ADCBUF0CHANNEL5;
    continuousConversion[5].sampleBuffer = &sampleBufferOne[5];
    continuousConversion[5].samplesRequestedCount = 1;

    if (!adcBuf) {
        /* AdcBuf did not open correctly. */
        while (1)
            ;
    }
}

/* This function converts a differential buffer to signed microvolts*/
int_fast16_t convertAdjustedDifferential(ADCBuf_Handle handle,

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

void read_adc(uint32_t *reading) {
    /* Blocking mode conversion */

    // This probably isn't how the multichannel reads are meant to work
    // But anything else (such as calling multiple conversions at once in
    // _convert result in a dma error
    for (int channel = 0; channel < 6; channel++) {
        int_fast16_t res =
            ADCBuf_convert(adcBuf, &continuousConversion[channel], 1);
        if (res == ADCBuf_STATUS_SUCCESS) {
            convertAdjustedDifferential(
                adcBuf, continuousConversion[channel].sampleBuffer,
                outputBuffer + channel, 1);
        }
    }
}
