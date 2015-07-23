/*
 * Application.h
 *
 *  Created on: Jul 15, 2015
 *      Author: XuanHung
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "chip.h"

#define CAR_HIT_CURRENT		10000

#define GATE_OPENED		0
#define GATE_CLOSED		1

void System_Init();
void System_Running();
void OpenGate();
void CloseGate();


void IncreaseCounterTimer();
void ResetCounterTimer();
uint32_t GetCounterTimer();
uint8_t CarHitDetection();
void ClearCarHitFlag();
void LCD_DisplayCurrent(uint16_t value);
void LCD_DisplayCounter(uint32_t value);

#endif /* INC_APPLICATION_H_ */
