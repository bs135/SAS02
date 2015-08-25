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

#define CURRENT_VERSION		9

#define DELTA_CURRENT_REFERENCE	600

#define TIME_CHECK_CARHIT	300

uint32_t MotorTotalTimer = 0;
uint32_t CloseDelayTimer = 0;
uint8_t VTimer_MotorTotalTimeout;
uint8_t VTimer_MotorDelayTimeout;
uint8_t VTimer_CarhitDelayTimeout;
uint8_t VTimer_ResetCalibratioID;
uint8_t GateState = 0;

uint32_t CounterTimer = 0;

uint8_t carhitDetectFlag = 0;

uint32_t TotalCounter  = 0;
uint32_t TotalCounterTemp  = 0;
uint8_t ProtectIndexTemp = 0;
uint32_t VersionNumber = CURRENT_VERSION;
uint8_t CarHitActive = 0;
uint8_t ResetCalibFlag = 0;
uint16_t CurrentReference = 0;
void System_Init(){
	VTimer_MotorTotalTimeout = VTimerGetID();
	VTimer_MotorDelayTimeout = VTimerGetID();
	VTimer_CarhitDelayTimeout = VTimerGetID();
	VTimer_ResetCalibratioID = VTimerGetID();
	LcdPrintVersion(CURRENT_VERSION);
	DelayMs(1000);
	EEPROMFirstCheck();
	DelayMs(10);
	CarHitActive = EEPROM_ReadByte(EEPROM_CARHIT_FLAG_ADDRESS);
	DelayMs(10);
	CurrentReference = EEPROM_ReadByte(EEPROM_CURRENT_HIGH_ADDRESS);
	DelayMs(10);
	CurrentReference = CurrentReference<<8;
	CurrentReference = CurrentReference | EEPROM_ReadByte(EEPROM_CURRENT_LOW_ADDRESS);
	DelayMs(10);

	ProtectIndexTemp = EEPROM_ReadByte(EEPROM_PROTECT_INDEX_ADDRESS);
	DelayMs(10);
	TotalCounter = EEPROMReadCycleCounter(ProtectIndexTemp);
	DelayMs(10);
	LCD_Clear();
	LCD_DisplayCounter(TotalCounter);
	LcdPrintString(0,0,"__");

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
	LcdPrintString(0,1,"0.0A");
	ClearCarHitFlag();
	FAN_TurnOff(1);
	Motor_Stop();
}


uint8_t SystemState = WAIT_BUTTON;
uint8_t CloseWhenOpenFlag = 0;
uint8_t ObjectDetectFlag = 0;
uint8_t SEN1State = 0;
uint8_t SEN2Flag = 0;
uint8_t UpdateLCDFlag = 0;
uint8_t CarReverseFlag = 0;
uint8_t SensorReverseFlag = 0;

uint8_t NumPressDownSwitch = 1;
void System_Running(){
	if (SWITCH_Pressed()){
		UART_SendString("SWITCH3 \r\n\t");
		VTimerSet(VTimer_ResetCalibratioID,5000);
		while (!VTimerIsFired(VTimer_ResetCalibratioID)){
			if (!(SWITCH_Pressed())){
				CalibartionProcess();
				ResetCalibFlag = 0;
				break;
			}
			else {
				ResetCalibFlag = 1;
			}
		}
		if (ResetCalibFlag == 1){
			ResetCalibartionProcess();
			ResetCalibFlag  = 0;
			DelayMs(2000);
		}
	}

	if (SWITCH3_GetEdgeStatus() == RISING_EDGE){ // Clear EEPROM
		UART_SendString("SWITCH3 \r\n\t");
		SWITCH3_ClearEdgeStatus();
		ClearCycleCounter();
		ProtectIndexTemp = EEPROM_ReadByte(EEPROM_PROTECT_INDEX_ADDRESS);
		DelayMs(10);
		TotalCounter = EEPROMReadCycleCounter(ProtectIndexTemp);
		DelayMs(10);
		LCD_DisplayCounter(TotalCounter);
		LcdPrintString(0,0,"__");
		LcdPrintString(0,1,"0.0A");
		UpdateLCDFlag = 1;
		LCD_DisplayInfo();
	}

	if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
		if (SEN1State == SEN1_STATE_NO) UpdateLCDFlag= 1;
		SEN1State = SEN1_STATE_NC;
		if (!SEN1_Pressed()){	// Detect object
			ObjectDetectFlag = 1;
		}
		else {
			ObjectDetectFlag = 0;
		}
	}
	else {
		if (SEN1State == SEN1_STATE_NC) UpdateLCDFlag= 1;
		SEN1State = SEN1_STATE_NO;
		if (SEN1_Pressed()){	// Detect object
			ObjectDetectFlag = 1;
		}
		else {
			ObjectDetectFlag = 0;
		}
	}

	if ((DIPSW_GetValue() & (1<<DIPSW3_INDEX)) == (1<<DIPSW3_INDEX)){	// ON
		if (CloseDelayTimer != 0) UpdateLCDFlag= 1;
		CloseDelayTimer = 0;
	}
	else if ((DIPSW_GetValue() & (1<<DIPSW2_INDEX)) == (1<<DIPSW2_INDEX)){ // ON
		if (CloseDelayTimer != 3000) UpdateLCDFlag= 1;
		CloseDelayTimer = 3000;
	}
	else {
		if (CloseDelayTimer != 1000) UpdateLCDFlag= 1;
		CloseDelayTimer = 1000;
	}

	if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
		if (MotorTotalTimer != 3000) UpdateLCDFlag= 1;
		MotorTotalTimer = 3000;
	}
	else {
		if (MotorTotalTimer != 6000) UpdateLCDFlag= 1;
		MotorTotalTimer = 6000;
	}

	if ((DIPSW_GetValue() & (1<<DIPSW5_INDEX)) == (1<<DIPSW5_INDEX)){	// Car Reverse
		if (CarReverseFlag != 1) UpdateLCDFlag= 1;
		CarReverseFlag = 1;
	}
	else {
		if (CarReverseFlag != 0) UpdateLCDFlag= 1;
		CarReverseFlag = 0;
	}

	if ((DIPSW_GetValue() & (1<<DIPSW6_INDEX)) == (1<<DIPSW6_INDEX)){	// Sensor Reverse
		if (SensorReverseFlag != 1) UpdateLCDFlag= 1;
		SensorReverseFlag = 1;
	}
	else {
		if (SensorReverseFlag != 0) UpdateLCDFlag= 1;
		SensorReverseFlag = 0;
	}

	LCD_DisplayInfo();
	switch (SystemState){
		case WAIT_BUTTON:
			if (UP_GetEdgeStatus() == FALLING_EDGE){	//UP_SW falling edge
				UpdateLCDFlag = 1;
				LCD_DisplayInfo();
				UP_ClearEdgeStatus();
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
			if (DOWN_GetEdgeStatus() == RISING_EDGE){	//DW_SW rising edge
				NumPressDownSwitch ++;
				if (NumPressDownSwitch >= 2){
					NumPressDownSwitch = 1;
					UpdateLCDFlag = 1;
					LCD_DisplayInfo();
					DOWN_ClearEdgeStatus();
					if (!LM_DOWN_Pressed()){
						VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
						SystemState = WAIT_OBJECT_REMOVE;
					}
				}
			}
			if (SEN2_GetEdgeStatus() == RISING_EDGE){	//rising edge
				UpdateLCDFlag = 1;
				LCD_DisplayInfo();
				SEN2_ClearEdgeStatus();
				if (!LM_DOWN_Pressed()){
					VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
					SystemState = WAIT_OBJECT_REMOVE;
				}
			}
			if (CloseWhenOpenFlag == 1){
				SystemState = WAIT_OBJECT_REMOVE;
			}
			break;
		case LEVER_MOVING_UP:
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){	// hết timeout mà chưa đụng LM_UP
				Motor_Stop();
				LcdPrintString(0,0,"__");
				SystemState = INCREASE_COUNTER;
				break;
			}
			if ((LM_UP_Pressed())){							// đụng được LM_UP
				SystemState = REACH_UP_LIMIT;
				break;
			}

			/*CalculateCurrentValue();
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
			}*/

			if (DOWN_GetEdgeStatus() == FALLING_EDGE){	//DW_SW falling edge
					DOWN_ClearEdgeStatus();
					CloseWhenOpenFlag = 1;
			}
			if (SEN2_GetEdgeStatus() == RISING_EDGE){	//SEN2 rising edge
					SEN2_ClearEdgeStatus();
					CloseWhenOpenFlag = 1;
			}
			else if (UP_Button_Pressed()){
				if (CloseWhenOpenFlag ==1){
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					CloseWhenOpenFlag = 0;
				}
			}
			break;
		case WAIT_OBJECT_REMOVE:
			if (UP_Button_Pressed()){		// Hủy lệnh đi xuống
				SystemState = WAIT_BUTTON;
				CloseWhenOpenFlag = 0;
			}
			else if (ObjectDetectFlag == 0){	// Không có vật cản
				VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
				SystemState = LEVER_WAIT_MOVE_DOWN;
			}
			break;
		case LEVER_WAIT_MOVE_DOWN:
				if (UP_Button_Pressed()){	// Hủy lệnh đi xuống
					SystemState = WAIT_BUTTON;
					CloseWhenOpenFlag = 0;
				}
				//else if (DOWN_Button_Pressed() || (!SEN2_Pressed())){
				//	OpenWhenCloseFlag = 0;
				//}
				else if (VTimerIsFired(VTimer_MotorDelayTimeout)){	// hết timeout delay motor
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
		case LEVER_MOVE_DOWN:
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){	// hết timeout mà chưa đụng DW_UP
				CloseWhenOpenFlag = 0;
				SystemState = RESET_VALUE;
				break;
			}
			if (!(LM_UP_Pressed())){		// Nếu rào chắn không phải đang ở tại UP_LM
				if (UP_Button_Pressed()){	// Hủy lệnh đi xuống
					CloseWhenOpenFlag = 0;
					Motor_Forward();		// Motor đi lên
					FAN_TurnOn();
					LcdPrintString(0,0,"UP");
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
					SystemState = LEVER_MOVING_UP;
					break;
				}
				else if (DOWN_GetEdgeStatus() == FALLING_EDGE){	// Cạnh xuống của tín hiện
					DOWN_ClearEdgeStatus();
					Motor_Forward();
					FAN_TurnOn();
					LcdPrintString(0,0,"UP");
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
					SystemState = LEVER_MOVING_UP;
					break;
				}
				else if (ObjectDetectFlag == 1){
					Motor_Forward();
					FAN_TurnOn();
					LcdPrintString(0,0,"UP");
					VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
					VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
					SystemState = LEVER_MOVING_UP;
					if (SensorReverseFlag == 1){
						CloseWhenOpenFlag = 0;
					}
					else {
						CloseWhenOpenFlag = 1;
					}
					break;
				}
			}
			if (CarHitActive == 1){
				if (VTimerIsFired(VTimer_CarhitDelayTimeout)){
					CalculateCurrentValue();
					if (calculate_done == 1){
						LCD_DisplayCurrent(GetCurrentValue());
						UART_SendNumber(CurrentReference);
						UART_SendByte(13);
						UART_SendNumber(GetCurrentValue());
						UART_SendByte(13);
						calculate_done = 0;
					}
					if (CarHitDetection()){
						Motor_Forward();
						FAN_TurnOn();
						LcdPrintString(0,0,"UP");
						VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
						SystemState = LEVER_MOVING_UP;
						NumPressDownSwitch = 0;
						carhitDetectFlag = 0;
						break;
					}
				}
			}

			if ((LM_DOWN_Pressed())){		// Đi xuống đụng DW_LM
				CloseWhenOpenFlag = 0;
				SystemState = REACH_DOWN_LIMIT;
				break;
			}

			break;
		case REACH_UP_LIMIT:
			Motor_Stop();
			LcdPrintString(0,0,"__");
			if (DownSwitchLevel == LOW_LEVEL){ // vẫn đè nút DOWN khi đụng UP_LM
				CloseWhenOpenFlag = 0;
			}
			SystemState = INCREASE_COUNTER;
			break;
		case REACH_DOWN_LIMIT:
			Motor_Stop();
			SystemState = RESET_VALUE;
			break;
		case INCREASE_COUNTER:
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
			SystemState = WAIT_BUTTON;
			break;
		case CAR_HIT_DETECT_UP:
			break;
		case CAR_HIT_DETECT_DOWN:
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
	if (GetCurrentValue() - CurrentReference > DELTA_CURRENT_REFERENCE){
		UART_SendString("CAR HIT\r\n\t");
		return 1;
	}
	return 0;
}
void ClearCarHitFlag(){
	carhitDetectFlag = 0;
}
void LCD_DisplayInfo(){
	if (UpdateLCDFlag == 1){
		LCD_Clear();
		LCD_DisplayCounter(TotalCounter);
		LcdPrintString(0,0,"__");
		LcdPrintString(0,1,"0.0A");
		if (SEN1State == SEN1_STATE_NC){
			LcdPrintString(11,1,"sNC");
		}
		else if (SEN1State == SEN1_STATE_NO){
			LcdPrintString(11,1,"sNO");
		}
		if (CloseDelayTimer == 0){
			LcdPrintString(8,1,"N-");
		}
		else if (CloseDelayTimer == 1000){
			LcdPrintString(8,1,"D1");
		}
		else if (CloseDelayTimer == 3000){
			LcdPrintString(8,1,"D3");
		}

		if (MotorTotalTimer == 3000){\
			LcdPrintString(5,1,"3S");
		}
		else if (MotorTotalTimer == 6000){\
			LcdPrintString(5,1,"6S");
		}

		if (CarReverseFlag == 1){
			LcdPrintString(5,0,"CR");
		}
		else {
			LcdPrintString(5,0,"  ");
		}

		if (SensorReverseFlag == 1){
			LcdPrintString(3,0,"SR");
		}
		else {
			LcdPrintString(3,0,"  ");
		}

		if (CarHitActive == 1){
			LcdPrintString(15,1,"C");
		}
		else {
			LcdPrintString(15,1," ");
		}
		UpdateLCDFlag = 0;
	}
}
void LCD_DisplayCurrent(uint16_t value){
	LCD_GotoXY(0,1);
	LcdPutChar('0' + ((value/1000) %10));
	LcdPutChar('.');
	LcdPutChar('0' + ((value/100) %10));
	LcdPutChar('A');
}
void LCD_DisplayCurrentAtPos(uint8_t col, uint8_t row,uint16_t value){
	LCD_GotoXY(col,row);
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
void EEPROMFirstCheck(){
	uint8_t retry = 0;
	uint8_t temp1, temp2;
	temp1 = 1;
	temp2 = 2;
	do {
		temp1 = EEPROM_ReadByte(EEPROM_CHECK_COUNTER_ADDRESS);
		UART_SendByte(temp1);
		UART_SendByte(' ');
		DelayMs(10);
		temp2 = EEPROM_ReadByte(EEPROM_CHECK_COUNTER_ADDRESS);
		UART_SendByte(temp2);
		UART_SendByte(13);
		DelayMs(10);
		retry ++;
	} while ((temp1 != temp2) && (retry < 3));

	if ((temp1 != 0x55) && (temp2 != 0x55)){
		//UART_SendString("FIRST CHECK\r\n\t");
		EEPROM_WriteByte(EEPROM_CHECK_COUNTER_ADDRESS,0x55);
		DelayMs(10);
		//temp1 = EEPROM_ReadByte(EEPROM_CHECK_COUNTER_ADDRESS);
		//DelayMs(10);
		//UART_SendByte(temp1);
		EEPROM_WriteByte(EEPROM_PROTECT_INDEX_ADDRESS,EEPROM_CYCLE_COUNTER_ADDRESS);
		DelayMs(10);
		//temp1 = EEPROM_ReadByte(EEPROM_PROTECT_INDEX_ADDRESS);
		//DelayMs(10);
		//UART_SendByte(temp1 + '0');
		ClearMeasureCurrent();
		EEPROMWriteCycleCounter(EEPROM_CYCLE_COUNTER_ADDRESS,0);
		DelayMs(10);
		//temp1 = EEPROM_ReadByte(EEPROM_CHECK_COUNTER_ADDRESS);
		//DelayMs(10);
		//UART_SendByte(temp1);
	}
}
void ClearCycleCounter(){
	EEPROM_WriteByte(EEPROM_PROTECT_INDEX_ADDRESS,EEPROM_CYCLE_COUNTER_ADDRESS);
	DelayMs(10);
	ProtectIndexTemp = EEPROM_ReadByte(EEPROM_PROTECT_INDEX_ADDRESS);
	DelayMs(10);
	LCD_Clear();
	LcdPrintString(0,0,"Waiting...");
	TotalCounter = 0;
	EEPROMWriteCycleCounter(ProtectIndexTemp,TotalCounter);
	DelayMs(10);
	LcdPrintString(0,0,"ERASE DONE");
	DelayMs(500);
	LCD_Clear();
	UpdateLCDFlag = 1;
}
void ClearMeasureCurrent(){
	LCD_Clear();
	LcdPrintString(0,0,"Waiting...");
	CurrentReference = 0;
	EEPROM_WriteByte(EEPROM_CARHIT_FLAG_ADDRESS,0);
	DelayMs(10);
	EEPROM_WriteByte(EEPROM_CURRENT_HIGH_ADDRESS,0);
	DelayMs(10);
	EEPROM_WriteByte(EEPROM_CURRENT_LOW_ADDRESS,0);
	DelayMs(10);
	LCD_Clear();
	LcdPrintString(0,0,"ERASE DONE");
	DelayMs(500);
	LCD_Clear();
	UpdateLCDFlag = 1;
}
void EEPROMWriteCycleCounter(uint8_t address,uint32_t _value){
	uint8_t temp[4];
	temp[0] = (uint8_t)(_value >> 24) & 0xFF;
	temp[1] = (uint8_t)(_value >> 16) & 0xFF;
	temp[2] = (uint8_t)(_value >> 8) & 0xFF;
	temp[3] = (uint8_t)_value & 0xFF;
	EEPROM_WriteByte(address,temp[0]);
	DelayMs(10);
	EEPROM_WriteByte(address+1,temp[1]);
	DelayMs(10);
	EEPROM_WriteByte(address+2,temp[2]);
	DelayMs(10);
	EEPROM_WriteByte(address+3,temp[3]);
	DelayMs(10);
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
void EEPROM_ClearAll(){
	uint16_t i;
	for (i=0;i<EEPROM_SIZE;i++){
		EEPROM_WriteByte(i,0);
		DelayMs(10);
	}
}
void EEPROM_EraseCurrentReference(){
	CurrentReference = 0;
	EEPROM_WriteByte(EEPROM_CURRENT_HIGH_ADDRESS,0);
	DelayMs(10);
	EEPROM_WriteByte(EEPROM_CURRENT_LOW_ADDRESS,0);
}

#define CURRENT_BUFFER_LENGTH	100
uint16_t CurrentValueTemp[CURRENT_BUFFER_LENGTH];
uint16_t CurrentValue[5] = {0,0,0,0,0};
void ClearCurrentBuffer(){
	uint8_t i;
	for (i=0;i<CURRENT_BUFFER_LENGTH;i++){
		CurrentValueTemp[i] = 0;
	}
}
uint16_t CalculateAverageCurrent(uint16_t* buffer,uint8_t length){
	uint8_t i;
	uint32_t temp_value = 0;
	for (i=0;i<length;i++){
		temp_value = temp_value + buffer[i];
	}
	temp_value = temp_value / length;
	return (uint16_t)temp_value;
}
uint8_t CheckDifferene(uint16_t* buffer, uint8_t _length){
	uint16_t ValueMax = buffer[0];
	uint16_t ValueMin = buffer[0];
	uint8_t i = 0;
	for (i=1;i<_length;i++){
		if (ValueMax < buffer[i]){
			ValueMax = buffer[i];
		}
		if (ValueMin > buffer[i]){
			ValueMin = buffer[i];
		}
	}
	if ((ValueMax - ValueMin) > 600)
		return 1;
	else
		return 0;
}
void CalibartionProcess(){
	uint8_t Cycle = 0;
	uint8_t StartAtDownFlag = 0;
	uint8_t Error = 0;
	uint8_t Times= 0;
	LCD_Clear();
	LcdPrintString(0,0,"CALIBRATION");
	LcdPrintString(0,1,"CYCLE=");
	LcdPutChar('0'+ Cycle);
	if (LM_DOWN_Pressed()){
		StartAtDownFlag = 1;
	}
	VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
	if (!LM_UP_Pressed()){
		Motor_Forward();
		FAN_TurnOn();
	}
	while (!LM_UP_Pressed()){	// Barrier move up if it is at DOWN Position
		if (VTimerIsFired(VTimer_MotorTotalTimeout)){
			Error = 1;
			break;
		}
	}
	Motor_Stop();
	// Barrier is at LM_UP
	Cycle = 0;
	while (Cycle < 5){
		if (Error == 1){
			break;
		}
		ClearCurrentBuffer();
		Motor_Reverse(); // Motor move DOWN
		VTimerSet(VTimer_CarhitDelayTimeout,TIME_CHECK_CARHIT);
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		while (!LM_DOWN_Pressed()){
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){
				Error = 1;
				break;
			}
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
				if (!SEN1_Pressed()){	// Detect object
					ObjectDetectFlag = 1;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}
			else {
				if (SEN1_Pressed()){	// Detect object
					ObjectDetectFlag = 1;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}
			if (ObjectDetectFlag == 1){		// SEN1 detected
				Error = 1;
				break;
			}
			CalculateCurrentValue();
			if (calculate_done == 1){
				if (VTimerIsFired(VTimer_CarhitDelayTimeout)){	// Motor Total timeout
					if (Times < 100){
						CurrentValueTemp[Times++] = GetCurrentValue();
						UART_SendNumber(GetCurrentValue());
						UART_SendByte(13);
					}
				}
				LCD_DisplayCurrentAtPos(10,1,GetCurrentValue());
				calculate_done = 0;
			}
		}
		Motor_Stop();
		CurrentValue[Cycle] = CalculateAverageCurrent(CurrentValueTemp,Times);
		CurrentReference = CurrentReference + CurrentValue[Cycle];
		Times = 0;
		DelayMs(300);
		if (Error == 0){
			Motor_Forward();	// Motor move UP
			VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
			while (!LM_UP_Pressed()){
				if (VTimerIsFired(VTimer_MotorTotalTimeout)){		// Motor Total timeout
					Error = 1;
					break;
				}
			}
			Motor_Stop();
			LcdPrintString(0,1,"CYCLE=");
			LcdPutChar('1'+ Cycle);
			DelayMs(300);
		}
		Cycle ++;
	}
	if (StartAtDownFlag == 1){
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		Motor_Reverse();
		while (!LM_DOWN_Pressed()){
			if (VTimerIsFired(VTimer_MotorTotalTimeout)){
				break;
			}
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
				if (!SEN1_Pressed()){	// Detect object
					ObjectDetectFlag = 1;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}
			else {
				if (SEN1_Pressed()){	// Detect object
					ObjectDetectFlag = 1;
				}
				else {
					ObjectDetectFlag = 0;
				}
			}
			if (ObjectDetectFlag == 1){		// SEN1 detected
				break;
			}
		}
	}
	Motor_Stop();
	FAN_TurnOff(60000);
	if (CheckDifferene(CurrentValue,5)){
		Error = 1;
	}
	if (Error == 0){
		CarHitActive = 1;
		EEPROM_WriteByte(EEPROM_CARHIT_FLAG_ADDRESS,1);
		DelayMs(10);
		CurrentReference = CurrentReference / 5;
		EEPROM_WriteByte(EEPROM_CURRENT_HIGH_ADDRESS,CurrentReference>>8);
		DelayMs(10);
		EEPROM_WriteByte(EEPROM_CURRENT_LOW_ADDRESS,CurrentReference & 0x00FF);
		LCD_Clear();
		LcdPrintString(0,0,"COMPLETE");
		LcdPrintString(0,1,"REFERENCE");
		LCD_DisplayCurrentAtPos(10,1,CurrentReference);
	}
	else {
		CarHitActive = 0;
		EEPROM_WriteByte(EEPROM_CARHIT_FLAG_ADDRESS,0);
		DelayMs(10);
		EEPROM_EraseCurrentReference();
		DelayMs(10);
		LCD_Clear();
		LcdPrintString(0,0,"ERROR");
		LCD_DisplayCurrentAtPos(7,0,CurrentValue[0]);
		LCD_DisplayCurrentAtPos(12,0,CurrentValue[1]);
		LCD_DisplayCurrentAtPos(2,1,CurrentValue[2]);
		LCD_DisplayCurrentAtPos(7,1,CurrentValue[3]);
		LCD_DisplayCurrentAtPos(12,1,CurrentValue[4]);
	}
}

void ResetCalibartionProcess(){
	LCD_Clear();
	LcdPrintString(0,0,"RESET");
	LcdPrintString(0,1,"CALIBRATION");
	CarHitActive = 0;
	EEPROM_WriteByte(EEPROM_CARHIT_FLAG_ADDRESS,0);
	DelayMs(10);
	EEPROM_EraseCurrentReference();
	DelayMs(10);
	LcdPrintString(15,1," ");
}
