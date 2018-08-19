#pragma once

#include <source/system_state.h>
#include <ti/drivers/UART.h>

void uartcallback(UART_Handle handle, void *buf, size_t count);
void receive_state_from_simulator();
void init_hil();
void send_state_to_simulator(SystemState state);
