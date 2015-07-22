/*
 * ADC.c
 *
 *  Created on: Jun 5, 2015
 *      Author: XuanHung
 */
#include "ADC.h"

static ADC_CLOCK_SETUP_T ADCSetup;

uint16_t dataADC;
uint8_t adc_cnt = 0;
uint16_t maxValue = 0;
uint8_t getCurrentFlag = 0;
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
	if (getCurrentFlag == 1){
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {};
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);
		UART_SendNumber(dataADC);
		UART_SendByte(13);
	}
}

uint16_t ADC_GetValue(){
	return dataADC;
}

void GetCurrentService(){
	//if (getCurrentFlag == 1){
	//	uint16_t GetCurentValue()
	//}

}

uint16_t GetCurentValue(){
	uint16_t _temp = 0;
	_temp = (GetMaxValue(dataADC) - 64);
	_temp = _temp * 37;
	return _temp;
}


uint16_t GetMaxValue(uint16_t a){
	if (maxValue < a)
		maxValue = a;
}
void ResetMaxValue(){
	maxValue = 0;
}
