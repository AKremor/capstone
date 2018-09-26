#include <source/hal/hil.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include "ti/devices/msp432e4/driverlib/driverlib.h"

void init_hil() {
    /* Enable the GPIO Peripheral used by the UART */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))) {
    }

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    /* Configure GPIO Pins for UART mode */
    MAP_GPIOPinConfigure(GPIO_PD4_U2RX);
    MAP_GPIOPinConfigure(GPIO_PD5_U2TX);
    MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    MAP_UARTConfigSetExpClk(
        UART2_BASE, 120E6, 500000,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
}

void send_state_to_simulator() {
    SystemState* state = SystemState::get();
    abc_quantity abc_reference = state->reference.get_abc();

    int8_t ref_val_a = abc_reference.a * 20;
    int8_t ref_val_b = abc_reference.b * 20;
    int8_t ref_val_c = abc_reference.c * 20;

    abc_quantity load_voltage = state->load_voltage.get_abc();
    int8_t load_voltage_an = (int8_t)(load_voltage.a * 10);
    int8_t load_voltage_bn = (int8_t)(load_voltage.b * 10);
    int8_t load_voltage_cn = (int8_t)(load_voltage.c * 10);

    abc_quantity load_ll_current = state->load_line_current.get_abc();
    int8_t load_ll_current_a = (int8_t)(load_ll_current.a * 64);
    int8_t load_ll_current_b = (int8_t)(load_ll_current.b * 64);
    int8_t load_ll_current_c = (int8_t)(load_ll_current.c * 64);

    int8_t ab_level =
        (int8_t)((state->a_phase - sizeof(svm_phase_levels_a) / 2) -
                 (state->b_phase - sizeof(svm_phase_levels_b) / 2));
    int8_t bc_level =
        (int8_t)((state->b_phase - sizeof(svm_phase_levels_b) / 2) -
                 (state->c_phase - sizeof(svm_phase_levels_c) / 2));
    int8_t ca_level =
        (int8_t)((state->c_phase - sizeof(svm_phase_levels_c) / 2) -
                 (state->a_phase - sizeof(svm_phase_levels_a) / 2));

    int8_t buffer[20] = {
        65,                                         // 0
        97,                                         // 1
        ab_level,                                   // 2
        bc_level,                                   // 3
        ca_level,                                   // 4
        ref_val_a,                                  // 5
        ref_val_b,                                  // 6
        ref_val_c,                                  // 7
        load_voltage_an,                            // 8
        load_voltage_bn,                            // 9
        load_voltage_cn,                            // 10
        load_ll_current_a,                          // 11
        load_ll_current_b,                          // 12
        load_ll_current_c,                          // 13
        (int8_t)(state->control_output.d * 10),     // 14
        (int8_t)(state->control_output.q * 10),     // 15
        (int8_t)(state->control_output.zero * 10),  // 16
        (int8_t)(state->pid_error.d * 20),          // 17
        (int8_t)(state->pid_error.q * 20),          // 18
        (int8_t)(state->pid_error.zero * 20)        // 19
    };

    for (uint8_t i = 0; i < sizeof(buffer); i++) {
        MAP_UARTCharPutNonBlocking(UART2_BASE, buffer[i]);
    }
}
