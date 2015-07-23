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

// TODO: insert other definitions and declarations here

uint16_t cnt = 0;
void SysTick_Handler(void)
{
	VTimerService();
	Input_Service();
	IncreaseCounterTimer();
	FanService();
	//ADCService();
	/*if (cnt>50){
		UART_SendNumber(dataADC);
		//UART_SendNumber(GetCurent());
		UART_SendByte(13);
		cnt = 0;
	}
	else {
		cnt++;
	}*/
}

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
}

 /* @brief	main routine
 * @return	Function should not exit.
 */
int main(void)
{
	uint8_t VT_ID ;
	SystemCoreClockUpdate();
	Board_Init();
	UART_SendString("SAS02\r\n\t");
	LcdPrintString(0,0,"HWBEMV1.0");
	System_Init();

	//VT_ID = VTimerGetID();
	//getCurrentFlag = 1;
	UART_SendString("START\r\n\t");
	while (1) {
		System_Running();
		/*VTimerSet(VT_ID,5000);
		Motor_Forward();
		DelayMs(100);
		ResetMaxValue();
		UART_SendString("FORWRD\r\n\t");
		while (!(VTimerIsFired(VT_ID))){
			CalculateCurentValue();
			if (GetCurrentValue() > CAR_HIT_CURRENT){
				UART_SendString("CAR HIT\r\n\t");
				ResetCurrentValue();
				break;
			}
			//UART_SendNumber(ADC_GetValue());
			//UART_SendByte(' ');

		//	DelayMs(100);
			//LcdPutDigi5(0,1,GetCurentValue());
			/hip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
			while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {};
			Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC1);
			GetMaxValue(dataADC1);
			GetMinValue(dataADC1);
			UART_SendNumber(dataADC1);
		/UART_SendByte(' ');
			//tem = (((uint32_t)dataADC1 - 65)*78 / 51 * 3300 / 1024)* 1000 / 133;	// mA

			//UART_SendNumber(tem);
			//UART_SendByte(13);
			count++;
			//DelayMs(1);
		}
		Motor_Stop();
		DelayMs(3000);
		UART_SendString("REVERSE\r\n\t");
		VTimerSet(VT_ID,5000);
		ResetMaxValue();
		Motor_Reverse();
		DelayMs(100);
		while (!(VTimerIsFired(VT_ID))){
			CalculateCurentValue();
			if (GetCurrentValue() > CAR_HIT_CURRENT){
				UART_SendString("CAR HIT\r\n\t");
				ResetCurrentValue();
				break;
			}
			//UART_SendNumber(ADC_GetValue());
			//UART_SendByte(' ');
			//UART_SendNumber(currentValue);
			//UART_SendByte(13);
			//LcdPutDigi4(0,0,currentValue);
			//DelayMs(100);
			//LcdPutDigi5(0,1,GetCurentValue());
			Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
			while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {};
			Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC1);
			//UART_SendNumber(dataADC1);
			//UART_SendByte(' ');
			GetMaxValue(dataADC1);
			GetMinValue(dataADC1);
			//tem = (((uint32_t)dataADC1 * 3300 / 1024) - 128) * 1000 / 133;
			//UART_SendNumber(tem);
			//UART_SendByte(13);
			//DelayMs(20);
			count++;
		}
		Motor_Stop();
		DelayMs(3000);
		/*UART_SendString("STOP\r\n\t");
		UART_SendNumber(min_current);
		UART_SendByte(' ');
		UART_SendNumber(max_current);
		UART_SendByte(13);
		tem = (((uint32_t)max_current - 60) * 5 )* 1000 / 133;	// mA
		UART_SendNumber(tem);
		UART_SendByte(13);
		max_current = 0;*/
		//DelayMs(1000);
		//Motor_Forward();
		//DelayMs(5000);
		//Motor_Reverse();
		//DelayMs(5000);
		//System_Running();
		//OpenGate();
		//CloseGate();
		/*LED_TurnOnUPSWLED();
		LED_TurnOnDWSWLED();
		FAN_TurnOn();
		Motor_Forward();

		DelayMs(1000);
		LED_TurnOffUPSWLED();
		LED_TurnOffDWSWLED();
		FAN_TurnOff();
		Motor_Reverse();
		DelayMs(1000);*/
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
		//Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

		/* Waiting for A/D conversion complete */
		//while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}

		/* Read ADC value */
		//Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);
		//if (UP_Button_Pressed()){
		//	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT0_BASE,0,7);
		//}
		//else {
		//	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT0_BASE,0,7);

		//}
		//DelayMs(500);
	}
	return 0;
}
