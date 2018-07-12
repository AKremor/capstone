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
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <unistd.h>
#include "arm_math.h"

/* Driver Header files */
#include <ti/drivers/GPIO.h>

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

    // TODO Probably a better way to handle this. Same with the peripheral
    // enables above
    GPIOPinTypeGPIOOutput(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SystemState desired_state = {1, 2, 0};
    SystemState system_state = {0.8, 0.2, 0};

    // Loop 1

    // Loop 2

    LoadModel load = {0, 50, 10e-3, 1e-6, 0};
    load.model_reciprocal_denominator = 1.0 / (load.R * load.Ts + load.L);

    for (int i = 0; i < 100000; i++) {
        // Run the optimiser once
        int level_index = findOptimalSwitchingIndex(
            &system_state, &desired_state, cell_states, &load);
        setGateSignals(cell_states[level_index]);
        system_state.current_alpha += 0.08;
        system_state.current_beta += 0.02;
    }

    // TODO Make sure to force output so no optim

    while (1) {
        sleep(time);
    }
}
}
