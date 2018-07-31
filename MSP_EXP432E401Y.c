/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== MSP_EXP432E401Y.c ========
 *  This file is responsible for setting up the board specific items for the
 *  MSP_EXP432E401Y board.
 */
#include <stdint.h>
#include <stdlib.h>

#ifndef __MSP432E401Y__
#define __MSP432E401Y__
#endif

#include <ti/devices/msp432e4/inc/msp432.h>

#include <ti/devices/msp432e4/driverlib/adc.h>
#include <ti/devices/msp432e4/driverlib/interrupt.h>
#include <ti/devices/msp432e4/driverlib/pwm.h>
#include <ti/devices/msp432e4/driverlib/sysctl.h>
#include <ti/devices/msp432e4/driverlib/udma.h>

#include <ti/drivers/Power.h>

#include "MSP_EXP432E401Y.h"

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCMSP432E4.h>

/* ADC objects */
ADCMSP432E4_Object adcMSP432E4Objects[MSP_EXP432E401Y_ADCCOUNT];

/* ADC configuration structure */
const ADCMSP432E4_HWAttrsV1 adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADCCOUNT] = {
    {.adcPin = ADCMSP432E4_PE_3_A0,
     .refVoltage = ADCMSP432E4_VREF_INTERNAL,
     .adcModule = ADCMSP432E4_MOD0,
     .adcSeq = ADCMSP432E4_SEQ0},
    {.adcPin = ADCMSP432E4_PE_2_A1,
     .refVoltage = ADCMSP432E4_VREF_INTERNAL,
     .adcModule = ADCMSP432E4_MOD1,
     .adcSeq = ADCMSP432E4_SEQ0}};

const ADC_Config ADC_config[MSP_EXP432E401Y_ADCCOUNT] = {
    {.fxnTablePtr = &ADCMSP432E4_fxnTable,
     .object = &adcMSP432E4Objects[MSP_EXP432E401Y_ADC0],
     .hwAttrs = &adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADC0]},
    {.fxnTablePtr = &ADCMSP432E4_fxnTable,
     .object = &adcMSP432E4Objects[MSP_EXP432E401Y_ADC1],
     .hwAttrs = &adcMSP432E4HWAttrs[MSP_EXP432E401Y_ADC1]}};

const uint_least8_t ADC_count = MSP_EXP432E401Y_ADCCOUNT;

/*
 *  =============================== ADCBuf ===============================
 */
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufMSP432E4.h>

/* ADC objects */
ADCBufMSP432E4_Object adcbufMSP432E4Objects[MSP_EXP432E401Y_ADCBUFCOUNT];

ADCBufMSP432E4_Channels
    adcBuf0MSP432E4Channels[MSP_EXP432E401Y_ADCBUF0CHANNELCOUNT] = {
        {.adcPin = ADCBufMSP432E4_PE_3_A0,
         .adcSequence = ADCBufMSP432E4_Seq_0,
         .adcInputMode = ADCBufMSP432E4_DIFFERENTIAL,
         .adcDifferentialPin = ADCBufMSP432E4_PE_2_A1,
         .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
         .refVoltage = 3300000},
        {.adcPin = ADCBufMSP432E4_PE_2_A1,
         .adcSequence = ADCBufMSP432E4_Seq_1,
         .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
         .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
         .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
         .refVoltage = 3300000},
        {.adcPin = ADCBufMSP432E4_PIN_NONE,
         .adcSequence = ADCBufMSP432E4_Seq_0,
         .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
         .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
         .adcInternalSource = ADCBufMSP432E4_TEMPERATURE_MODE,
         .refVoltage = 3300000},

        {.adcPin = ADCBufMSP432E4_PE_3_A0,
         .adcSequence = ADCBufMSP432E4_Seq_0,
         .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
         .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
         .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
         .refVoltage = 3300000},
        {.adcPin = ADCBufMSP432E4_PE_2_A1,
         .adcSequence = ADCBufMSP432E4_Seq_0,
         .adcInputMode = ADCBufMSP432E4_SINGLE_ENDED,
         .adcDifferentialPin = ADCBufMSP432E4_PIN_NONE,
         .adcInternalSource = ADCBufMSP432E4_INTERNAL_SOURCE_MODE_OFF,
         .refVoltage = 3300000}};

/* ADC sequencer priorities for SS0-SS3, set to 0-3 to initialize sequencer */
static ADCBufMSP432E4_SequencePriorities
    seqPriorities[ADCBufMSP432E4_SEQUENCER_COUNT] = {
        ADCBufMSP432E4_Priority_0, ADCBufMSP432E4_Seq_Disable,
        ADCBufMSP432E4_Seq_Disable, ADCBufMSP432E4_Seq_Disable};

/* ADC sequencer tigger source for SS0-SS3*/
static ADCBufMSP432E4_TriggerSource
    triggerSource[ADCBufMSP432E4_SEQUENCER_COUNT] = {
        ADCBufMSP432E4_TIMER_TRIGGER, ADCBufMSP432E4_TIMER_TRIGGER,
        ADCBufMSP432E4_TIMER_TRIGGER, ADCBufMSP432E4_TIMER_TRIGGER};

/* ADC configuration structure */
const ADCBufMSP432E4_HWAttrsV1
    adcbufMSP432E4HWAttrs[MSP_EXP432E401Y_ADCBUFCOUNT] = {{
        .intPriority = ~0,
        .adcBase = ADC0_BASE,
        .channelSetting = adcBuf0MSP432E4Channels,
        .sequencePriority = seqPriorities,
        .adcTriggerSource = triggerSource,
        .modulePhase = ADCBufMSP432E4_Phase_Delay_0,
        .refSource = ADCBufMSP432E4_VREF_INTERNAL,
        .useDMA = 1,
        .adcTimerSource = TIMER3_BASE,
    }};

const ADCBuf_Config ADCBuf_config[MSP_EXP432E401Y_ADCBUFCOUNT] = {
    {.fxnTablePtr = &ADCBufMSP432E4_fxnTable,
     .object = &adcbufMSP432E4Objects[MSP_EXP432E401Y_ADCBUF0],
     .hwAttrs = &adcbufMSP432E4HWAttrs[MSP_EXP432E401Y_ADCBUF0]}};

const uint_least8_t ADCBuf_count = MSP_EXP432E401Y_ADCBUFCOUNT;

/*
 *  =============================== DMA ===============================
 */
#include <ti/drivers/dma/UDMAMSP432E4.h>

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 1024
#elif defined(__GNUC__)
__attribute__((aligned(1024)))
#endif
static tDMAControlTable dmaControlTable[64];

/*
 *  ======== dmaErrorFxn ========
 *  This is the handler for the uDMA error interrupt.
 */
static void dmaErrorFxn(uintptr_t arg) {
    int status = uDMAErrorStatusGet();
    uDMAErrorStatusClear();

    /* Suppress unused variable warning */
    (void)status;

    while (1)
        ;
}

UDMAMSP432E4_Object udmaMSP432E4Object;

const UDMAMSP432E4_HWAttrs udmaMSP432E4HWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMAMSP432E4_ErrorFxn)dmaErrorFxn,
    .intNum = INT_UDMAERR,
    .intPriority = (~0)};

const UDMAMSP432E4_Config UDMAMSP432E4_config = {
    .object = &udmaMSP432E4Object, .hwAttrs = &udmaMSP432E4HWAttrs};

/*
 *  =============================== General ===============================
 */
/*
 *  ======== MSP_EXP432E401Y_initGeneral ========
 */
void MSP_EXP432E401Y_initGeneral(void) {
    Power_init();

    /* Grant the DMA access to all FLASH memory */
    FLASH_CTRL->PP |= FLASH_PP_DFA;

    /* Region start address - match FLASH start address */
    FLASH_CTRL->DMAST = 0x00000000;

    /*
     * Access to FLASH is granted to the DMA in 2KB regions.  The value
     * assigned to DMASZ is the amount of 2KB regions to which the DMA will
     * have access.  The value can be determined via the following:
     *     2 * (num_regions + 1) KB
     *
     * To grant full access to entire 1MB of FLASH:
     *     2 * (511 + 1) KB = 1024 KB (1 MB)
     */
    FLASH_CTRL->DMASZ = 511;
}

/*
 *  =============================== GPIO ===============================
 */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432E4.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432E401Y.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {

};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432E401Y.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {

};

/* The device-specific GPIO_config structure */
const GPIOMSP432E4_Config GPIOMSP432E4_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    .numberOfCallbacks =
        sizeof(gpioCallbackFunctions) / sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)};

/*
 *  =============================== I2C ===============================
 */
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432E4.h>

I2CMSP432E4_Object i2cMSP432E4Objects[MSP_EXP432E401Y_I2CCOUNT];

const I2CMSP432E4_HWAttrs i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2CCOUNT] = {
    {.baseAddr = I2C0_BASE,
     .intNum = INT_I2C0,
     .intPriority = (~0),
     .sclPin = I2CMSP432E4_PB2_I2C0SCL,
     .sdaPin = I2CMSP432E4_PB3_I2C0SDA},
    {.baseAddr = I2C7_BASE,
     .intNum = INT_I2C7,
     .intPriority = (~0),
     .sclPin = I2CMSP432E4_PD0_I2C7SCL,
     .sdaPin = I2CMSP432E4_PD1_I2C7SDA}};

const I2C_Config I2C_config[MSP_EXP432E401Y_I2CCOUNT] = {
    {.fxnTablePtr = &I2CMSP432E4_fxnTable,
     .object = &i2cMSP432E4Objects[MSP_EXP432E401Y_I2C0],
     .hwAttrs = &i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2C0]},
    {.fxnTablePtr = &I2CMSP432E4_fxnTable,
     .object = &i2cMSP432E4Objects[MSP_EXP432E401Y_I2C7],
     .hwAttrs = &i2cMSP432E4HWAttrs[MSP_EXP432E401Y_I2C7]},
};

const uint_least8_t I2C_count = MSP_EXP432E401Y_I2CCOUNT;

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/power/PowerMSP432E4.h>
const PowerMSP432E4_Config PowerMSP432E4_config = {
    .policyFxn = &PowerMSP432E4_sleepPolicy, .enablePolicy = true};

/*
 *  =============================== PWM ===============================
 */
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMMSP432E4.h>

PWMMSP432E4_Object pwmMSP432E4Objects[MSP_EXP432E401Y_PWMCOUNT];

const PWMMSP432E4_HWAttrs pwmMSP432E4HWAttrs[MSP_EXP432E401Y_PWMCOUNT] = {
    {.pwmBaseAddr = PWM0_BASE,
     .pwmOutput = PWM_OUT_0,
     .pwmGenOpts = PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN,
     .pinConfig = PWMMSP432E4_PF0_M0PWM0}};

const PWM_Config PWM_config[MSP_EXP432E401Y_PWMCOUNT] = {
    {.fxnTablePtr = &PWMMSP432E4_fxnTable,
     .object = &pwmMSP432E4Objects[MSP_EXP432E401Y_PWM0],
     .hwAttrs = &pwmMSP432E4HWAttrs[MSP_EXP432E401Y_PWM0]},
};

const uint_least8_t PWM_count = MSP_EXP432E401Y_PWMCOUNT;

/*
 *  =============================== Timer ===============================
 */
#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerMSP432E4.h>

TimerMSP432E4_Object timerMSP432E4Objects[MSP_EXP432E401Y_TIMERCOUNT];

const TimerMSP432E4_HWAttrs timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMERCOUNT] = {
    {.baseAddress = TIMER2_BASE,
     .subTimer = TimerMSP432E4_timer32,
     .intNum = INT_TIMER2A,
     .intPriority = ~0},
    {.baseAddress = TIMER1_BASE,
     .subTimer = TimerMSP432E4_timer16A,
     .intNum = INT_TIMER1A,
     .intPriority = ~0},
    {.baseAddress = TIMER1_BASE,
     .subTimer = TimerMSP432E4_timer16B,
     .intNum = INT_TIMER1B,
     .intPriority = ~0},
};

const Timer_Config Timer_config[MSP_EXP432E401Y_TIMERCOUNT] = {
    {.fxnTablePtr = &TimerMSP432E4_fxnTable,
     .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER0],
     .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER0]},
    {.fxnTablePtr = &TimerMSP432E4_fxnTable,
     .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER1],
     .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER1]},
    {.fxnTablePtr = &TimerMSP432E4_fxnTable,
     .object = &timerMSP432E4Objects[MSP_EXP432E401Y_TIMER2],
     .hwAttrs = &timerMSP432E4HWAttrs[MSP_EXP432E401Y_TIMER2]},
};

const uint_least8_t Timer_count = MSP_EXP432E401Y_TIMERCOUNT;

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432E4.h>

UARTMSP432E4_Object uartMSP432E4Objects[MSP_EXP432E401Y_UARTCOUNT];
unsigned char uartMSP432E4RingBuffer[MSP_EXP432E401Y_UARTCOUNT][32];

/* UART configuration structure */
const UARTMSP432E4_HWAttrs uartMSP432E4HWAttrs[MSP_EXP432E401Y_UARTCOUNT] = {
    {.baseAddr = UART0_BASE,
     .intNum = INT_UART0,
     .intPriority = (~0),
     .flowControl = UARTMSP432E4_FLOWCTRL_NONE,
     .ringBufPtr = uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART0],
     .ringBufSize = sizeof(uartMSP432E4RingBuffer[MSP_EXP432E401Y_UART0]),
     .rxPin = UARTMSP432E4_PA0_U0RX,
     .txPin = UARTMSP432E4_PA1_U0TX,
     .ctsPin = UARTMSP432E4_PIN_UNASSIGNED,
     .rtsPin = UARTMSP432E4_PIN_UNASSIGNED,
     .errorFxn = NULL}};

const UART_Config UART_config[MSP_EXP432E401Y_UARTCOUNT] = {
    {.fxnTablePtr = &UARTMSP432E4_fxnTable,
     .object = &uartMSP432E4Objects[MSP_EXP432E401Y_UART0],
     .hwAttrs = &uartMSP432E4HWAttrs[MSP_EXP432E401Y_UART0]}};

const uint_least8_t UART_count = MSP_EXP432E401Y_UARTCOUNT;
