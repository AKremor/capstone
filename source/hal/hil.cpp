#include <source/hal/hil.h>
#include <source/init.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include "ti/devices/msp432e4/driverlib/driverlib.h"

void init_hil() {
    /* Enable the GPIO Peripheral used by the UART */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))) {
    }

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    /* Configure GPIO Pins for UART mode */
    MAP_GPIOPinConfigure(GPIO_PJ0_U3RX);
    MAP_GPIOPinConfigure(GPIO_PJ1_U3TX);
    MAP_GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    MAP_UARTConfigSetExpClk(
        UART3_BASE, 120E6, 921600,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
}

void send_state_to_simulator() {
    int8_t buffer[46];
    buffer[0] = 'A';
    buffer[1] = 'a';
    buffer[2] = 43;  // Length to come after this byte
    memcpy(buffer + 3, &Id_ref, 4);
    memcpy(buffer + 7, &Iq_ref, 4);
    memcpy(buffer + 11, &I_Aa, 4);
    memcpy(buffer + 15, &I_Bb, 4);
    memcpy(buffer + 19, &I_Cc, 4);
    memcpy(buffer + 23, &V_an, 4);
    memcpy(buffer + 27, &V_bn, 4);
    memcpy(buffer + 31, &V_cn, 4);
    memcpy(buffer + 35, &Id_error, 4);
    memcpy(buffer + 39, &Iq_error, 4);
    memcpy(buffer + 43, &level_9_detect, 1);
    memcpy(buffer + 44, &level_3_detect, 1);
    memcpy(buffer + 45, &level_1_detect, 1);

    for (uint8_t i = 0; i < sizeof(buffer); i++) {
        MAP_UARTCharPut(UART3_BASE, buffer[i]);
    }
}

constexpr int32_t command_code_set_magnitude = 0x01;
constexpr int32_t command_code_set_frequency = 0x02;

void receive_uart() {
    // Sync char 1
    if ('A' != UARTCharGetNonBlocking(UART3_BASE)) {
        return;
    }

    if ('a' != UARTCharGet(UART3_BASE)) {
        return;
    }

    int32_t length = UARTCharGet(UART3_BASE);
    int32_t command_code = UARTCharGet(UART3_BASE);

    int32_t buffer[16];

    for (int i = 0; i < length; i++) {
        buffer[i] = UARTCharGet(UART3_BASE);
    }

    switch (command_code) {
        case command_code_set_magnitude: {
            Id_ref = (float)(buffer[0] << 24 | buffer[1] << 16 |
                             buffer[2] << 8 | buffer[3] << 0);
            break;
        }
        case command_code_set_frequency: {
            fundamental_frequency_hz =
                (float)(buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 |
                        buffer[3] << 0);
            break;
        }
    }
}
