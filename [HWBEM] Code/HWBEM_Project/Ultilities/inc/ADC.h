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
uint16_t ADC_GetValue(void);
uint16_t GetCurent();
#endif /* INC_ADC_H_ */
