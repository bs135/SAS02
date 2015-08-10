/*
 * Application.c
 *
 *  Created on: Jul 15, 2015
 *      Author: XuanHung
 */

#include "Application.h"
#include "Input.h"
#include "UART.h"
#include "Motor.h"
#include "VTimer.h"
#include "ADC.h"
#include "LCD.h"
#include "EEPROM.h"
#include "LCD.h"

#define CURRENT_VERSION		7

#define TIME_CHECK_CARHIT	300

uint32_t MotorTotalTimer = 0;
uint32_t CloseDelayTimer = 0;
uint8_t VTimer_MotorTotalTimeout;
uint8_t VTimer_MotorDelayTimeout;
uint8_t VTimer_CarhitDelayTimeout;
uint8_t GateState = 0;

uint32_t CounterTimer = 0;

uint8_t carhitDetectFlag = 0;

uint32_t TotalCounter  = 0;
uint32_t TotalCounterTemp  = 0;
uint8_t ProtectIndexTemp = 0;
uint32_t VersionNumber = CURRENT_VERSION;

void System_Init(){
	uint8_t dummy_data[5] = {0,0,0,0,0};
	VTimer_MotorTotalTimeout = VTimerGetID();
	VTimer_MotorDelayTimeout = VTimerGetID();
	VTimer_CarhitDelayTimeout = VTimerGetID();
	if (EEPROM_ReadByte(EEPROM_CHECK_COUNTER_ADDRESS) != 0x55){
		EEPROM_WriteByte(EEPROM_CHECK_COUNTER_ADDRESS,0x55);
		DelayMs(10);
		EEPROM_WriteByte(EEPROM_PROTECT_INDEX_ADDRESS,2);
		DelayMs(10);
		EEPROMWriteCycleCounter(EEPROM_CYCLE_COUNTER_ADDRESS,0);
		DelayMs(10);
	}
	//EEPROM_WriteByte(EEPROM_PROTECT_INDEX_ADDRESS,2);
	//DelayMs(10);
	ProtectIndexTemp = EEPROM_ReadByte(EEPROM_PROTECT_INDEX_ADDRESS);
	TotalCounter = EEPROMReadCycleCounter(ProtectIndexTemp);
	LCD_Clear();
	LCD_DisplayCounter(TotalCounter);
	LcdPrintString(0,0,"__");
	LcdPrintString(2,0,"0.00S");
	if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){
		LcdPrintString(5,1,"3S");
	}
	else {
		LcdPrintString(5,1,"6S");
	}

	if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
		LcdPrintString(11,1,"sNC");
	}
	else {
		LcdPrintString(11,1,"sNO");
	}
	if ((DIPSW_GetValue() & (1<<DIPSW3_INDEX)) == (1<<DIPSW3_INDEX)){	// ON
		LcdPrintString(8,1,"N-");
	}
	else {
		LcdPrintString(8,1,"D1");
	}
	ResetCurrentValue();
	LCD_DisplayCurrent(GetCurrentValue());
	ClearCarHitFlag();
	FAN_TurnOff(1);
	Motor_Stop();

}


uint8_t SystemState = WAIT_BUTTON;
uint8_t CloseWhenOpenFlag = 0;
uint8_t OpenWhenCloseFlag = 0;
uint8_t OpenSuccess = 0;
uint8_t CloseSuccess = 0;
uint8_t dipSW23_value = 0;
uint8_t ObjectDetectFlag = 0;
uint8_t SEN1State = 0;
uint8_t SEN2Flag = 0;
void System_Running(){
	if (SWITCH_Pressed()){
		LCD_Clear();
		LcdPrintVersion(VersionNumber);
		DelayMs(3000);
		LCD_Clear();
		LcdPrintString(0,0,"__");
		LcdPrintString(2,0,"0.00S");
		LCD_DisplayCounter(TotalCounter);
		LCD_DisplayCurrent(GetCurrentValue());
	}
	if (SystemState == WAIT_BUTTON){
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
			SEN1State = SEN1_STATE_NC;
			LcdPrintString(11,1,"sNC");
		}
		else {
			SEN1State = SEN1_STATE_NO;
			LcdPrintString(11,1,"sNO");
		}

		if ((DIPSW_GetValue() & (1<<DIPSW3_INDEX)) == (1<<DIPSW3_INDEX)){	// ON
			CloseDelayTimer = 0;
			LcdPrintString(8,1,"N-");
		}
		else if ((DIPSW_GetValue() & (1<<DIPSW2_INDEX)) == (1<<DIPSW2_INDEX)){ // ON
			CloseDelayTimer = 3000;
			LcdPrintString(8,1,"D3");
		}
		else {
			CloseDelayTimer = 1000;
			LcdPrintString(8,1,"D1");
		}

		if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
			MotorTotalTimer = 3000;
			LcdPrintString(5,1,"3S");
		}
		else {
			MotorTotalTimer = 6000;
			LcdPrintString(5,1,"6S");
		}
	}
	switch (SystemState){
		case WAIT_BUTTON:
			if (UP_Button_Pressed()){
				if (!LM_UP_Pressed()){
					LcdPrintString(0,0,"UP");
					Motor_Forward();
					FAN_TurnOn();
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					ResetCounterTimer();
					VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
					SystemState = LEVER_MOVING_UP;
					CalculateCurrentValue();
				}
				else {
					LcdPrintString(0,0,"__");
					Motor_Stop();
				}
			}
			//else if (DOWN_Button_Pressed()){
			else if (DOWN_GetEdgeStatus() == RISING_EDGE){
				DownSwitchEdgeStatus =  HIGH_NO_EDGE;
				if (!LM_DOWN_Pressed()){
					//FAN_TurnOn();
					VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
					SystemState = WAIT_OBJECT_REMOVE;
				}

			}
			else if (SEN2HoldFlag == 0){
				if (!(SEN2_Pressed())){
					//LcdPrintString(0,0,"DW");
					FAN_TurnOn();
					VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
					SystemState = WAIT_OBJECT_REMOVE;
				}
			}
			else if (ObjectDetectFlag == 1){
				SystemState = WAIT_OBJECT_REMOVE;
			}
			break;
		case LEVER_MOVING_UP:
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){
				Motor_Stop();
				LcdPrintString(0,0,"__");
				if (ObjectDetectFlag == 1){
					if (SEN1State == SEN1_STATE_NC){
						if (SEN1_Pressed()){	// Object is removed
							VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
							SystemState = LEVER_WAIT_MOVE_DOWN;
							ObjectDetectFlag = 0;
							break;
						}
					}
					else if (SEN1State == SEN1_STATE_NO){
						if (!SEN1_Pressed()){	// Object is removed
							VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
							SystemState = LEVER_WAIT_MOVE_DOWN;
							ObjectDetectFlag = 0;
							break;
						}
					}
				}
				else {
					LCD_PrintTime(2,0,GetCounterTimer());
					SystemState = INCREASE_COUNTER;
					Motor_Stop();
					LcdPrintString(0,0,"__");
				}
				break;
			}
			if ((LM_UP_Pressed())){
				LCD_PrintTime(2,0,GetCounterTimer());
				SystemState = REACH_UP_LIMIT;
				break;
			}
			CalculateCurrentValue();
			if (calculate_done == 1){
				LCD_DisplayCurrent(GetCurrentValue());
				calculate_done = 0;
			}
			if (carhitDetectFlag == 0){
				if (VTimerIsFired(VTimer_CarhitDelayTimeout)){
					carhitDetectFlag = 1;
				}
			}
			else if (carhitDetectFlag == 1){
				if (CarHitDetection()){
					//SystemState = CAR_HIT_DETECT_UP;
					//break;
					carhitDetectFlag = 0;
				}
			}
			if (DOWN_Button_Pressed()|| (!SEN2_Pressed())){
				ResetCounterTimer();
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				CloseWhenOpenFlag = 1;
			}
			else if (UP_Button_Pressed()){
				if (CloseWhenOpenFlag ==1){
					LCD_PrintTime(2,0,GetCounterTimer());
					ResetCounterTimer();
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					CloseWhenOpenFlag = 0;
				}
			}
			break;
		case LEVER_WAIT_MOVE_DOWN:
			if (UP_Button_Pressed()){
				OpenWhenCloseFlag = 1;
			}
			else if (DOWN_Button_Pressed()|| (!SEN2_Pressed())){
				OpenWhenCloseFlag = 0;
			}
			if (VTimerIsFired(VTimer_MotorDelayTimeout)){
				ResetCounterTimer();
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				if ((!LM_DOWN_Pressed())){
					LcdPrintString(0,0,"DW");
					Motor_Reverse();
					FAN_TurnOn();
				}
				else {
					LcdPrintString(0,0,"__");
					Motor_Stop();
				}
				CalculateCurrentValue();
				VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
				SystemState = LEVER_MOVE_DOWN;
			}
			break;
		case WAIT_OBJECT_REMOVE:
			if (SEN1State == SEN1_STATE_NC){
				if (SEN1_Pressed()){	// Object is removed
					VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
					SystemState = LEVER_WAIT_MOVE_DOWN;
					ObjectDetectFlag = 0;
					break;
				}
			}
			else if (SEN1State == SEN1_STATE_NO){
				if (!SEN1_Pressed()){	// Object is removed
					VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
					SystemState = LEVER_WAIT_MOVE_DOWN;
					ObjectDetectFlag = 0;
					break;
				}
			}
			break;
		case LEVER_MOVE_DOWN:
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){
				LCD_PrintTime(2,0,GetCounterTimer());
				SystemState = RESET_VALUE;
				break;
			}
			if (UP_Button_Pressed()|| (DOWN_Button_Pressed())){ // DW falling
				DownSwitchEdgeStatus =  HIGH_NO_EDGE;
				Motor_Forward();
				FAN_TurnOn();
				LcdPrintString(0,0,"UP");
				ResetCounterTimer();
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
				SystemState = LEVER_MOVING_UP;
				break;
			}
			if (SEN1State == SEN1_STATE_NC){
				if (!SEN1_Pressed()){	// Detect object
					if (LM_UP_Pressed()||LM_DOWN_Pressed()){
						Motor_Stop();
					}
					else {
						Motor_Forward();
						FAN_TurnOn();
					}
					LcdPrintString(0,0,"UP");
					SystemState = LEVER_MOVING_UP;
					ObjectDetectFlag = 1;
					CloseWhenOpenFlag = 1;
					break;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}
			else if (SEN1State == SEN1_STATE_NO){
				if (SEN1_Pressed()){	// Detect object
					if (LM_UP_Pressed()||LM_DOWN_Pressed()){
						LcdPrintString(0,0,"__");
						Motor_Stop();
					}
					else {
						LcdPrintString(0,0,"UP");
						Motor_Forward();
						FAN_TurnOn();
					}
					SystemState = LEVER_MOVING_UP;
					ObjectDetectFlag = 1;
					CloseWhenOpenFlag = 1;
					break;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}

			CalculateCurrentValue();
			if (calculate_done == 1){
				LCD_DisplayCurrent(GetCurrentValue());
				calculate_done = 0;
			}
			if (carhitDetectFlag == 0){
				if (VTimerIsFired(VTimer_CarhitDelayTimeout)){
					carhitDetectFlag = 1;
				}
			}
			else if (carhitDetectFlag == 1){
				if (CarHitDetection()){
					//SystemState = CAR_HIT_DETECT_DOWN;
					//break;
					carhitDetectFlag = 0;
				}
			}

			if ((LM_DOWN_Pressed())){
				LCD_PrintTime(2,0,GetCounterTimer());
				SystemState = REACH_DOWN_LIMIT;
				break;
			}
			break;
		case REACH_UP_LIMIT:
			Motor_Stop();
			if (CloseWhenOpenFlag == 1){
				VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
				SystemState = WAIT_OBJECT_REMOVE;
				Motor_Stop();
				LcdPrintString(0,0,"__");
				CloseWhenOpenFlag = 0;
				ObjectDetectFlag = 0;
			}
			else if (CloseWhenOpenFlag == 0){
				Motor_Stop();
				LcdPrintString(0,0,"__");
				SystemState = INCREASE_COUNTER;
			}
			else if (ObjectDetectFlag == 1){
				SystemState = WAIT_OBJECT_REMOVE;
			}
			break;
		case REACH_DOWN_LIMIT:
			Motor_Stop();
			if (OpenWhenCloseFlag == 1){
				OpenWhenCloseFlag = 0;
				LcdPrintString(0,0,"UP");
				Motor_Forward();
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				ResetCounterTimer();
				VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
				CalculateCurrentValue();
				SystemState = LEVER_MOVING_UP;
			}
			else {
				Motor_Stop();
				FAN_TurnOff(60000);
				SystemState = RESET_VALUE;
			}
			break;
		case CAR_HIT_DETECT_UP:
			break;
		case CAR_HIT_DETECT_DOWN:
			break;
		case INCREASE_COUNTER:
			if (ObjectDetectFlag == 0){
				TotalCounter ++;
				if ((TotalCounter % 500000) == 0){
					ProtectIndexTemp = ProtectIndexTemp + 4;
					if (ProtectIndexTemp > 200){
						ProtectIndexTemp = 2;
					}
					EEPROM_WriteByte(EEPROM_PROTECT_INDEX_ADDRESS,ProtectIndexTemp);
					DelayMs(10);
				}
				EEPROMWriteCycleCounter(ProtectIndexTemp,TotalCounter);
				LCD_DisplayCounter(TotalCounter);
			}
			SystemState = RESET_VALUE;
			break;

		case RESET_VALUE:
			Motor_Stop();
			FAN_TurnOff(60000);
			LcdPrintString(0,0,"__");
			ResetCurrentValue();
			LCD_DisplayCurrent(GetCurrentValue());
			carhitDetectFlag = 0;
			calculate_done = 0;
			CloseWhenOpenFlag = 0;
			SEN2HoldFlag = 1;
			SystemState = WAIT_BUTTON;
			break;
		default:
			break;

	}
}

void IncreaseCounterTimer(){
	CounterTimer ++;
}
void ResetCounterTimer(){
	CounterTimer = 0;
}
uint32_t GetCounterTimer(){
	return CounterTimer;
}

uint8_t CarHitDetection(){
	if (carhitDetectFlag == 1){
		if (GetCurrentValue() > CAR_HIT_CURRENT){
			//UART_SendString("CAR HIT\r\n\t");
			return 1;
		}
	}
	return 0;
}
void ClearCarHitFlag(){
	carhitDetectFlag = 0;
}

void LCD_DisplayCurrent(uint16_t value){
	LCD_GotoXY(0,1);
	LcdPutChar('0' + ((value/1000) %10));
	LcdPutChar('.');
	LcdPutChar('0' + ((value/100) %10));
	LcdPutChar('A');
}

void LCD_DisplayCounter(uint32_t value){
	LCD_GotoXY(8,0);
	LcdPutChar('0' + ((value/10000000) %10));
	LcdPutChar('0' + ((value/1000000) %10));
	LcdPutChar('0' + ((value/100000) %10));
	LcdPutChar('0' + ((value/10000) %10));
	LcdPutChar('0' + ((value/1000) %10));
	LcdPutChar('0' + ((value/100) %10));
	LcdPutChar('0' + ((value/10) %10));
	LcdPutChar('0' + (value %10));
}

void LcdPrintVersion(uint32_t version){
	LcdPrintString(0,0,"HWBEM v1.0");
	LcdPrintString(0,1,"Fw:");
	LCD_GotoXY(4,1);
	LcdPutChar('0' + ((version/100000) %10));
	LcdPutChar('0' + ((version/10000) %10));
	LcdPutChar('0' + ((version/1000) %10));
	LcdPutChar('0' + ((version/100) %10));
	LcdPutChar('0' + ((version/10) %10));
	LcdPutChar('0' + (version %10));
}

void EEPROMWriteCycleCounter(uint8_t address,uint32_t _value){
	uint8_t temp[4];
	temp[0] = (uint8_t)(_value >> 24) & 0xFF;
	temp[1] = (uint8_t)(_value >> 16) & 0xFF;
	temp[2] = (uint8_t)(_value >> 8) & 0xFF;
	temp[3] = (uint8_t)_value & 0xFF;
	EEPROM_WriteBytes(address,temp,4);
}

uint32_t EEPROMReadCycleCounter(uint8_t address){
	uint32_t temp_value = 0;
	uint8_t temp[4];
	EEPROM_ReadBytes(address,temp,4);
	temp_value = temp[0];
	temp_value = temp_value << 8;
	temp_value |= temp[1];
	temp_value = temp_value << 8;
	temp_value |= temp[2];
	temp_value = temp_value << 8;
	temp_value |= temp[3];
	return temp_value;
}

