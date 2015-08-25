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
#include "LCD.h"
#include "VTimer.h"
#include "LED.h"
#include "Motor.h"
#include "UART.h"
#include "ADC.h"
#include "Input.h"
#include "Application.h"
#include "EEPROM.h"

// TODO: insert other definitions and declarations here

uint16_t cnt = 0;
void SysTick_Handler(void)
{
	VTimerService();
	Input_Service();
	IncreaseCounterTimer();
	FanService();
	if (cnt>500){
		LED_ToggleUserLED();
		//UART_SendNumber(dataADC);
		//UART_SendNumber(GetCurent());
		//UART_SendByte(13);
		cnt = 0;
	}
	else {
		cnt++;
	}
}

void Board_Init(){

	SysTick_Config(SystemCoreClock / SYSTICK_RATE);
	VTimer_InitController();
	LED_InitController();
	Motor_InitController();
	LCD_InitController();
	UART_InitController();
	ADC_InitController();
	Input_InitController();
	I2C_InitController();
}

 /* @brief	main routine
 * @return	Function should not exit.
 */
int main(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	DelayMs(500);
	//UART_SendString("SAS02\r\n\t");
	System_Init();
	while (1) {
		System_Running();
	}
	return 0;
}
