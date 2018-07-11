/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== empty.c ========
 */

#include <src/mpc/mpc.h>
/* For usleep() */
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>


/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header file */
#include "Board.h"

/*
 *  ======== mainThread ========
 */
extern "C" {
void *mainThread(void *arg0) {
  /* 1 second delay */
  uint32_t time = 1;

  /* Call driver init functions */
  GPIO_init();
  // I2C_init();
  // SDSPI_init();
  // SPI_init();
  // UART_init();
  // Watchdog_init();

  /* Configure the LED pin */
  GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

  /* Turn on user LED */
  //GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

  /* Configure Port N pin 1 as output. */
      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
      while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)));
      GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);



  DesiredState desired_state;
  desired_state.desired_current_alpha = 1;
  desired_state.desired_current_beta = 2;
  desired_state.desired_current_zero = 0;
  SystemState system_state;
  system_state.current_alpha = 0.8;
  system_state.current_beta = 0.2;
  system_state.current_zero = 0;

  int total_switch = 0;
  /* Turn on user LED */
  GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

  for (int i = 0; i < 100000; i++) {
    // Run the optimiser once
     total_switch += findOptimalSwitchingIndex(system_state, desired_state, cell_states);

     for(int j = 0; j < 27; j++)
    system_state.current_alpha += 0.08;
    system_state.current_beta += 0.02;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
  }

  if(total_switch > 123){
      GPIO_toggle(Board_GPIO_LED0);
  } else {
      GPIO_toggle(Board_GPIO_LED1);
  }


  while (1) {
    sleep(time);
    GPIO_toggle(Board_GPIO_LED0);
  }
}
}
