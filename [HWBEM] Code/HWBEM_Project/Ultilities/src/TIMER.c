#include "TIMER.h"

void Timer_InitController(){

}

void TIMER16_0_InitController(){

}
void TIMER16_1_InitController(){

}
void TIMER32_0_InitController(){
	uint32_t timerFreq;
	/* Enable timer 0 clock */
	Chip_TIMER_Init(LPC_TIMER32_0);

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER32_0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
	Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / TICKRATE_TIM32_0));
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
	Chip_TIMER_Enable(LPC_TIMER32_0);

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
	NVIC_EnableIRQ(TIMER_32_0_IRQn);
}
void TIMER32_1_InitController(){

}

