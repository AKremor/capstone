#pragma once

extern "C" {
void TIMER0A_IRQHandler(void);
void TIMER1A_IRQHandler(void);
void TIMER2A_IRQHandler(void);
void TIMER3A_IRQHandler(void);
void TIMER4A_IRQHandler(void);
void ADC0SS2_IRQHandler(void);
}

void svm_control_loop();

// Globals
extern float Id_ref, Iq_ref;

extern float V_an, V_bn, V_cn;
extern float I_Aa, I_Bb, I_Cc;
