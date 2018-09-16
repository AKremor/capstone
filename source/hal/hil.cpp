#include <source/hal/hil.h>
#include <source/system_config.h>
#include <source/system_state.h>
#include "ti/devices/msp432e4/driverlib/driverlib.h"

void receive_state_from_simulator() {
    /* TODO(akremor)
    SystemState* state = SystemState::get();
    int8_t read_buffer[20];

    int8_t header[1] = {NULL};

    bool has_sync1 = false;

    while (!has_sync1) {
        int32_t ret = UART_read(uart, header, 1);

        if (ret == UART_STATUS_ERROR || ret == 0) {
            return;
        }
        if (header[0] == 'A') {
            has_sync1 = true;
        }
    }

    bool has_sync2 = false;

    while (!has_sync2) {
        int32_t ret = UART_read(uart, header, 1);

        if (ret == UART_STATUS_ERROR || ret == 0) {
            return;
        }
        if (header[0] == 'a') {
            has_sync2 = true;
        }
    }

    // Sync'd
    int32_t ret = UART_read(uart, read_buffer, 9);

    if (ret) {
        abc_quantity load_voltage = {(float32_t)read_buffer[0],
                                     (float32_t)read_buffer[1],
                                     (float32_t)read_buffer[2]};
        abc_quantity load_line_current = {(float32_t)(read_buffer[3] / 64.0),
                                          (float32_t)(read_buffer[4] / 64.0),
                                          (float32_t)(read_buffer[5] / 64.0)};
        abc_quantity load_ll_voltage = {(float32_t)read_buffer[6],
                                        (float32_t)read_buffer[7],
                                        (float32_t)read_buffer[8]};

        state->load_voltage.set_abc(load_voltage);
        state->load_line_current.set_abc(load_line_current);
        state->load_ll_voltage.set_abc(load_ll_voltage);
    }
    */
}

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
        UART2_BASE, 120E6, 921600,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
}

void send_state_to_simulator() {
    SystemState* state = SystemState::get();
    float scaling_factor = 20;
    abc_quantity abc_reference = state->reference.get_abc();

    int8_t ref_val_a = abc_reference.a * scaling_factor;
    int8_t ref_val_b = abc_reference.b * scaling_factor;
    int8_t ref_val_c = abc_reference.c * scaling_factor;

    int8_t a_9_cell = ((svm_phase_levels_a[state->a_phase] & A_POS9) ? 1 : 0) +
                      ((svm_phase_levels_a[state->a_phase] & A_NEG9) ? -1 : 0);
    int8_t a_3_cell = ((svm_phase_levels_a[state->a_phase] & A_POS3) ? 1 : 0) +
                      ((svm_phase_levels_a[state->a_phase] & A_NEG3) ? -1 : 0);
    int8_t a_1_cell = ((svm_phase_levels_a[state->a_phase] & A_POS1) ? 1 : 0) +
                      ((svm_phase_levels_a[state->a_phase] & A_NEG1) ? -1 : 0);

    int8_t b_9_cell = ((svm_phase_levels_b[state->b_phase] & B_POS9) ? 1 : 0) +
                      ((svm_phase_levels_b[state->b_phase] & B_NEG9) ? -1 : 0);
    int8_t b_3_cell = ((svm_phase_levels_b[state->b_phase] & B_POS3) ? 1 : 0) +
                      ((svm_phase_levels_b[state->b_phase] & B_NEG3) ? -1 : 0);
    int8_t b_1_cell = ((svm_phase_levels_b[state->b_phase] & B_POS1) ? 1 : 0) +
                      ((svm_phase_levels_b[state->b_phase] & B_NEG1) ? -1 : 0);

    int8_t c_9_cell = ((svm_phase_levels_c[state->c_phase] & C_POS9) ? 1 : 0) +
                      ((svm_phase_levels_c[state->c_phase] & C_NEG9) ? -1 : 0);
    int8_t c_3_cell = ((svm_phase_levels_c[state->c_phase] & C_POS3) ? 1 : 0) +
                      ((svm_phase_levels_c[state->c_phase] & C_NEG3) ? -1 : 0);
    int8_t c_1_cell = ((svm_phase_levels_c[state->c_phase] & C_POS1) ? 1 : 0) +
                      ((svm_phase_levels_c[state->c_phase] & C_NEG1) ? -1 : 0);

    abc_quantity load_voltage = state->load_voltage.get_abc();
    int8_t load_voltage_an = (int8_t)(load_voltage.a * 10);
    int8_t load_voltage_bn = (int8_t)(load_voltage.b * 10);
    int8_t load_voltage_cn = (int8_t)(load_voltage.c * 10);

    abc_quantity load_ll_current = state->load_line_current.get_abc();
    int8_t load_ll_current_a = (int8_t)(load_ll_current.a * 64);
    int8_t load_ll_current_b = (int8_t)(load_ll_current.b * 64);
    int8_t load_ll_current_c = (int8_t)(load_ll_current.c * 64);

    int8_t buffer[29] = {
        65,  // 0
        97,  // 1
        (int8_t)((state->a_phase - sizeof(svm_phase_levels_a) / 2) -
                 (state->b_phase - sizeof(svm_phase_levels_b) / 2)),  // 2
        (int8_t)((state->b_phase - sizeof(svm_phase_levels_b) / 2) -
                 (state->c_phase - sizeof(svm_phase_levels_c) / 2)),  // 3
        (int8_t)((state->c_phase - sizeof(svm_phase_levels_c) / 2) -
                 (state->a_phase - sizeof(svm_phase_levels_a) / 2)),  // 4
        ref_val_a,                                                    // 5
        ref_val_b,                                                    // 6
        ref_val_c,                                                    // 7
        a_9_cell,                                                     // 8
        a_3_cell,                                                     // 9
        a_1_cell,                                                     // 10
        b_9_cell,                                                     // 11
        b_3_cell,                                                     // 12
        b_1_cell,                                                     // 13
        c_9_cell,                                                     // 14
        c_3_cell,                                                     // 15
        c_1_cell,                                                     // 16
        load_voltage_an,                                              // 17
        load_voltage_bn,                                              // 18
        load_voltage_cn,                                              // 19
        load_ll_current_a,                                            // 20
        load_ll_current_b,                                            // 21
        load_ll_current_c,                                            // 22
        state->control_output.d * 10,                                 // 23
        state->control_output.q * 10,                                 // 24
        state->control_output.zero * 10,                              // 25
        state->pid_error.d * 20,                                      // 26
        state->pid_error.q * 20,                                      // 27
        state->pid_error.zero * 20                                    // 28
    };

    for (int i = 0; i < sizeof(buffer); i++) {
        MAP_UARTCharPutNonBlocking(UART2_BASE, buffer[i]);
    }
}
