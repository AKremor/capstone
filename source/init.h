

extern "C" {
void TIMER0A_IRQHandler(void);
void TIMER1A_IRQHandler(void);
void TIMER2A_IRQHandler(void);
void SysTick_Handler(void);
}

void svm_control_loop();
