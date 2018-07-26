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

#include <ti/drivers/Timer.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>

/* Board Header file */
#include <ti/sysbios/knl/Task.h>
#include "Board.h"

/*
 *  ======== mainThread ========
 */

void initSineWaveOutput();

extern "C" {

/* Callback used for toggling the LED. */
void timerCallback(Timer_Handle myHandle);

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

    // GPIOPadConfigSet(GPIO_PORTL_BASE, 0xFF, GPIO_STRENGTH_12MA,
    //                 GPIO_PIN_TYPE_STD);
    // TODO Probably a better way to handle this. Same with the peripheral
    // enables above
    GPIOPinTypeGPIOOutputOD(
        GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(
        GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                             GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutputOD(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // initSineWaveOutput();

    Timer_init();

    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = 9 * 1000;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (timer0 == NULL) {
        while (1)
            ;
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        while (1)
            ;
    }

    while (true)
        ;

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
        system_state.current_alpha += 0.008;
        system_state.current_beta += 0.002;
    }

    // TODO Make sure to force output so no optim

    while (1) {
        sleep(time);
    }
}

volatile uint64_t state_counter = 0;

// PL0 9VL POS
// PL1 9VR NEG
// PL2 3VL POS
// PL3 3VR NEG

#define POS9 0x01
#define NEG9 0x02
#define POS3 0x04
#define NEG3 0x08
#define OFF9 0x00
#define OFF3 0x00

uint8_t states[] = {NEG9 | NEG3, NEG9 | OFF3, NEG9 | POS3,
                    OFF9 | NEG3, OFF9 | OFF3, OFF9 | POS3,
                    POS9 | NEG3, POS9 | OFF3, POS9 | POS3};

int findNearestStateIndex(uint64_t state_counter) {
    uint8_t middle_offset = sizeof(states) / 2;

    double sine_val = (sizeof(states) / 2) * sin(1.0 * state_counter / 20);
    int level = floor(sine_val + 0.5) + middle_offset;
    return level;
}
void timerCallback(Timer_Handle myHandle) {
    int nearest_state_index = findNearestStateIndex(state_counter);
    GPIOPinWrite(GPIO_PORTL_BASE, 0b1111, states[nearest_state_index]);
    state_counter++;
}
}
