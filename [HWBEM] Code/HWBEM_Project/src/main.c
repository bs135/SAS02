/*
===============================================================================
 Name        : HWBEM_Project.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "main.h"
#include "ControlLCD.h"
#include "VTimer.h"
#include "LED.h"
#include "Motor.h"
#include "UART.h"
#include "ADC.h"

// TODO: insert other definitions and declarations here

uint16_t cnt = 0;
void SysTick_Handler(void)
{
	VTimerService();
	if (cnt>500){
	//	Chip_GPIO_SetPinToggle(LPC_GPIO_PORT0_BASE,0,7);
		cnt = 0;
	}
	else {
		cnt++;
	}
}

/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
void TIMER32_0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
		//Board_LED_Set(0, true);
	}
}

void Board_Init(){
	SysTick_Config(SystemCoreClock / SYSTICK_RATE);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,0,7);
	VTimer_InitController();
	LED_InitController();
	Motor_InitController();
	//LCD_InitController();
	UART_InitController();
	ADC_InitController();

}

 /* @brief	main routine
 * @return	Function should not exit.
 */
int main(void)
{
	uint16_t number = 0;
	uint16_t dataADC;
	SystemCoreClockUpdate();
	Board_Init();
	while (1) {
		//__WFI();
		//Chip_GPIO_SetPinToggle(LPC_GPIO_PORT2_BASE,0,7);
		//LCD_GotoXY(0,0);
		//LCD_PutDigi3(0,0,number++);
		/*LED_TurnOnDWSWLED();
		LED_TurnOnUPSWLED();
		LED_TurnOnDWLMLED();
		LED_TurnOnUPLMLED();
		LED_TurnOnUserLED();
		UART_SendByte('h');
		DelayMs(500);
		LED_TurnOffDWSWLED();
		LED_TurnOffUPSWLED();
		LED_TurnOffDWLMLED();
		LED_TurnOffUPLMLED();
		LED_TurnOffUserLED();
		UART_SendByte(13);
		DelayMs(500);*/
	//	if (UART_RXAvailable()){
	//		UART_SendByte(UART_ReceiveByte());
	//	}
		/* Start A/D conversion */
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

		/* Read ADC value */
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);
		UART_SendNumber(dataADC);
		UART_SendByte(13);
		DelayMs(50);
	}

	return 0;
}
