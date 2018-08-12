/** ============================================================================
 *  @file       MSP_EXP432E401Y.h
 *
 *  @brief      MSP_EXP432E401Y Board Specific APIs
 *
 *  The MSP_EXP432E401Y header file should be included in an application as
 *  follows:
 *  @code
 *  #include <MSP_EXP432E401Y.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef __MSP_EXP432E401Y_H
#define __MSP_EXP432E401Y_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Board_UARTName {
    UMBILICAL_SIM = 0,
    Board_UARTCOUNT
} Board_UARTName;

/*!
 *  @def    MSP_EXP432E401Y_ADCName
 *  @brief  Enum of ADC channels on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCName {
    MSP_EXP432E401Y_ADC0 = 0,
    MSP_EXP432E401Y_ADC1,

    MSP_EXP432E401Y_ADCCOUNT
} MSP_EXP432E401Y_ADCName;

/*!
 *  @def    MSP_EXP432E401Y_ADCBufName
 *  @brief  Enum of ADC hardware peripherals on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCBufName {
    MSP_EXP432E401Y_ADCBUF0 = 0,

    MSP_EXP432E401Y_ADCBUFCOUNT
} MSP_EXP432E401Y_ADCBufName;

/*!
 *  @def    MSP_EXP432E401Y_ADCBuf0ChannelName
 *  @brief  Enum of ADCBuf channels on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_ADCBuf0ChannelName {
    MSP_EXP432E401Y_ADCBUF0CHANNEL0 = 0,
    MSP_EXP432E401Y_ADCBUF0CHANNEL1,
    MSP_EXP432E401Y_ADCBUF0CHANNEL2,
    MSP_EXP432E401Y_ADCBUF0CHANNEL3,
    MSP_EXP432E401Y_ADCBUF0CHANNEL4,
    MSP_EXP432E401Y_ADCBUF0CHANNEL5,

    MSP_EXP432E401Y_ADCBUF0CHANNELCOUNT
} MSP_EXP432E401Y_ADCBuf0ChannelName;

/*!
 *  @def    MSP_EXP432E401Y_I2CName
 *  @brief  Enum of I2C names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_I2CName {
    MSP_EXP432E401Y_I2C0 = 0,
    MSP_EXP432E401Y_I2C7,

    MSP_EXP432E401Y_I2CCOUNT
} MSP_EXP432E401Y_I2CName;

/*!
 *  @def    MSP_EXP432E401Y_PWMName
 *  @brief  Enum of PWM names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_PWMName {
    MSP_EXP432E401Y_PWM0 = 0,

    MSP_EXP432E401Y_PWMCOUNT
} MSP_EXP432E401Y_PWMName;

/*!
 *  @def    MSP_EXP432E401Y_TimerName
 *  @brief  Enum of Timer names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_TimerName {
    MSP_EXP432E401Y_TIMER0 = 0,
    MSP_EXP432E401Y_TIMER1,
    MSP_EXP432E401Y_TIMER2,

    MSP_EXP432E401Y_TIMERCOUNT
} MSP_EXP432E401Y_TimerName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 *  This includes:
 *     - Enable clock sources for peripherals
 */
extern void MSP_EXP432E401Y_initGeneral(void);

#ifdef __cplusplus
}
#endif

#endif /* __MSP_EXP432E401Y_H */
