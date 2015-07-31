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
	//ADCService();
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
/*
void I2C_IRQHandler(void)
{
	Chip_I2C_MasterStateHandler(I2C0);
}*/
/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
/*
void TIMER32_0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
	}
}
*/
void Board_Init(){

	SysTick_Config(SystemCoreClock / SYSTICK_RATE);
	VTimer_InitController();
	LED_InitController();
	Motor_InitController();
	LCD_InitController();
	UART_InitController();
	ADC_InitController();
	Input_InitController();
	//EEPROM_InitController();
	//I2CInit(I2CMASTER);
	I2C_InitController();
}

 /* @brief	main routine
 * @return	Function should not exit.
 */
int main(void)
{
	uint8_t temp;
	SystemCoreClockUpdate();
	Board_Init();
	UART_SendString("SAS02\r\n\t");
	//LcdPrintString(0,0,"HWBEMV1.0");
	//System_Init();

	UART_SendString("START\r\n\t");
	temp = 150;
	UART_SendNumber(temp);
	//EEPROM_WriteBytes(3,temp);
	I2C_SendByte(0,temp);

	UART_SendString("WRITE\r\n\r\n\t");
	DelayMs(10);
	temp = 5;
	//temp = EEPROM_ReadBytes(3);
	I2C_ReadByte(0,&temp);
	UART_SendString("READ\r\n\t");
	UART_SendNumber(temp);
	while (1) {
		//System_Running();
		/*I2C_ReadByte(3,&temp);
		UART_SendString("READ\r\n\t");
		UART_SendNumber(temp);
		DelayMs(100);*/
	}
	return 0;
}
