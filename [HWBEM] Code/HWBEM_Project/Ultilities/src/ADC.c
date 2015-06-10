/*
 * ADC.c
 *
 *  Created on: Jun 5, 2015
 *      Author: XuanHung
 */
#include "ADC.h"

static ADC_CLOCK_SETUP_T ADCSetup;

void ADC_InitController(){
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, FUNC2);
	// Sample Rate : 400000
	// Resolution  : 10 bit
	// Burst mode  : False
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
}
