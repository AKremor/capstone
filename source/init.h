

extern "C" {
void TIMER0A_IRQHandler(void);
void TIMER1A_IRQHandler(void);
void TIMER2A_IRQHandler(void);
void TIMER3A_IRQHandler(void);
void TIMER4A_IRQHandler(void);
void ADC0SS2_IRQHandler(void);
}

void svm_control_loop();
