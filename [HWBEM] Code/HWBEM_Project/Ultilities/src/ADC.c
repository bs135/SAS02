/*
 * ADC.c
 *
 *  Created on: Jun 5, 2015
 *      Author: XuanHung
 */
#include "ADC.h"

static ADC_CLOCK_SETUP_T ADCSetup;

uint16_t dataADC;

void ADC_InitController(){
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, FUNC2);
	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, FUNC2);
	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_11, FUNC2);
	// Sample Rate : 400000
	// Resolution  : 10 bit
	// Burst mode  : False
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	//Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
	//Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);


}

uint16_t ADC_GetValue(){
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH1, ADC_DR_DONE_STAT) != SET) {}
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH1, &dataADC);
	return dataADC;
}

uint16_t GetCurent(){
	return (((uint32_t)ADC_GetValue() *3300 / 1024) - 210) * 1000 / 133;
}
