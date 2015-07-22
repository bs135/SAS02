/*
 * ADC.h
 *
 *  Created on: Jun 5, 2015
 *      Author: XuanHung
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "chip.h"

void ADC_InitController(void);
void ADCService(void);
uint16_t ADC_GetValue(void);
uint16_t GetCurentValue();
uint16_t GetMaxValue(uint16_t a);
void ResetMaxValue();

extern uint16_t maxValue;
extern uint8_t getCurrentFlag;
#endif /* INC_ADC_H_ */
