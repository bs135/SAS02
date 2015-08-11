/*
 * Application.h
 *
 *  Created on: Jul 15, 2015
 *      Author: XuanHung
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "chip.h"

#define CAR_HIT_CURRENT			3050

#define WAIT_BUTTON				0
#define LEVER_MOVING_UP			1
#define LEVER_WAIT_MOVE_DOWN	2
#define LEVER_MOVE_DOWN			3
#define REACH_UP_LIMIT			4
#define REACH_DOWN_LIMIT		5
#define CAR_HIT_DETECT_UP		6
#define CAR_HIT_DETECT_DOWN		7
#define INCREASE_COUNTER		8
#define RESET_VALUE				9
#define WAIT_OBJECT_REMOVE		10


#define SEN1_STATE_NO 			0
#define SEN1_STATE_NC 			1

#define GATE_OPENED				0
#define GATE_CLOSED				1

#define EEPROM_CHECK_COUNTER_ADDRESS	0
#define EEPROM_PROTECT_INDEX_ADDRESS	1
#define EEPROM_CYCLE_COUNTER_ADDRESS	2

void System_Init();
void System_Running();
void OpenGate();
void CloseGate();


void IncreaseCounterTimer();
void ResetCounterTimer();
uint32_t GetCounterTimer();
uint8_t CarHitDetection();
void ClearCarHitFlag();
void LCD_DisplayInfo();
void LCD_DisplayCurrent(uint16_t value);
void LCD_DisplayCounter(uint32_t value);
void LcdPrintVersion(uint32_t version);
void EEPROMFirstCheck();
void EEPROMWriteCycleCounter(uint8_t address,uint32_t _value);
uint32_t EEPROMReadCycleCounter(uint8_t address);

#endif /* INC_APPLICATION_H_ */
