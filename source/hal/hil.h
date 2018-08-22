#pragma once

#include <ti/drivers/UART.h>

void receive_state_from_simulator();
void init_hil();
void send_state_to_simulator();
void uartcallback(UART_Handle handle, void *buf, size_t count);
