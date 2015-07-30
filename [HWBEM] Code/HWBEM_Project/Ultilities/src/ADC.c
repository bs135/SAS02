/*
 * ADC.c
 *
 *  Created on: Jun 5, 2015
 *      Author: XuanHung
 */
#include "ADC.h"
#include "UART.h"
#include <stdlib.h>

static ADC_CLOCK_SETUP_T ADCSetup;

uint16_t dataADC = 0;
uint16_t adc_cnt = 0;
uint16_t maxValue = 0;
uint16_t currentValue = 0;
int32_t value_temp = 0;
uint8_t calculate_done = 0;

void ADC_InitController(){
	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_11, FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, FUNC2);
	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, FUNC2);

	// Sample Rate : 400000
	// Resolution  : 10 bit
	// Burst mode  : False
	Chip_ADC_Init(LPC_ADC, &ADCSetup);

	//Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	//Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
	//Chip_ADC_SetBurstCmd(LPC_ADC,ENABLE);

}
void ADCService(){

}

void CalculateCurrentValue(){
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {};
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);
	FindMaxValue(dataADC);
	adc_cnt ++;
	if (adc_cnt >= 3000){
		calculate_done = 1;
		value_temp = (maxValue - 120);
		//value_temp = abs(value_temp);
		currentValue = value_temp * 37;
		adc_cnt = 0;
		ResetMaxValue();
		//UART_SendNumber(dataADC);
		//UART_SendByte(13);
		//UART_SendNumber(currentValue);
		//UART_SendByte(13);
		//LcdPutDigi4(0,0,currentValue);
	}
}
uint16_t GetCurrentValue(){
	return currentValue;
}
void ResetCurrentValue(){
	currentValue = 0;
}
void FindMaxValue(uint16_t a){
	if (a >=120){
		if (maxValue < a)
			maxValue = a;
	}
}
uint16_t GetMaxValue(){
	return maxValue;
}
void ResetMaxValue(){
	maxValue = 0;
}
