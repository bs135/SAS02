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
void CalculateCurrentValue();
uint16_t GetCurrentValue();
void ResetCurrentValue();
void FindMaxValue(uint16_t a);
uint16_t GetMaxValue();
void ResetMaxValue();

extern uint8_t calculate_done;
#endif /* INC_ADC_H_ */
