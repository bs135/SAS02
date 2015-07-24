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

#define TIME_CHECK_CARHIT	300

uint32_t MotorTotalTimer = 0;
uint32_t CloseDelayTimer = 0;
uint8_t VTimer_MotorTotalTimeout;
uint8_t VTimer_MotorDelayTimeout;
uint8_t GateState = 0;

uint32_t CounterTimer = 0;

uint8_t carhitDetectFlag = 0;

uint32_t TotalCounter  = 0;

void System_Init(){
	VTimer_MotorTotalTimeout = VTimerGetID();
	VTimer_MotorDelayTimeout = VTimerGetID();
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
	if (!LM_DOWN_Pressed()){
		GateState = GATE_CLOSED;
		UART_SendString("GATE_CLOSED\r\n\t");
	}
	else if (!LM_UP_Pressed()){
		GateState = GATE_OPENED;
		UART_SendString("GATE_OPENED\r\n\t");
	}
	else {
		GateState = GATE_CLOSED;
		Motor_Reverse();
		FAN_TurnOn();
		VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
		UART_SendString("Testing\r\n\t");
		while (LM_DOWN_Pressed()){
			CalculateCurrentValue();
			if (calculate_done == 1){
				LCD_DisplayCurrent(GetCurrentValue());
				calculate_done = 0;
			}
			//LCD_DisplayCurrent(1234);
			if (carhitDetectFlag == 0){
				if (VTimerIsFired(VTimer_MotorDelayTimeout)){
					carhitDetectFlag = 1;
				}
			}
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// N/C
				if((!SEN1_Pressed()) || CarHitDetection()){	// Detect object
					Motor_Forward();
					while (LM_UP_Pressed()){
					}
					Motor_Stop();
					GateState = GATE_OPENED;
					break;
				}
			}
			else {
				if((SEN1_Pressed()) || CarHitDetection()){	// Detect object
					Motor_Forward();
					while (LM_UP_Pressed()){
					}
					Motor_Stop();
					GateState = GATE_OPENED;
					break;
				}
			}
		}
	}
	ResetCurrentValue();
	LCD_DisplayCurrent(GetCurrentValue());
	ClearCarHitFlag();
	FAN_TurnOff(1);
	Motor_Stop();

}

void System_Running(){
	uint8_t OpenSuccess = 0;
	uint8_t CloseSuccess = 0;
	uint8_t dipSW23_value = 0;
	uint8_t ObjectDetectFlag = 0;
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
	if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
		MotorTotalTimer = 3000;
		LcdPrintString(5,1,"3S");
	}
	else {
		MotorTotalTimer = 6000;
		LcdPrintString(5,1,"6S");
	}
	//if (GateState == GATE_CLOSED){
	//	OpenGate();
	//}
	//else if (GateState == GATE_OPENED){
	//	CloseGate();
	//}
	if (UP_Button_Pressed()){
		LcdPrintString(0,0,"UP");
		FAN_TurnOn();
		Motor_Forward();
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		ResetCounterTimer();
		VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
		while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
			CalculateCurrentValue();
			if (calculate_done == 1){
				LCD_DisplayCurrent(GetCurrentValue());
				LCD_PrintTime(2,0, MotorTotalTimer - GetCounterTimer());
				calculate_done = 0;
			}
			if (carhitDetectFlag == 0){
				if (VTimerIsFired(VTimer_MotorDelayTimeout)){
					carhitDetectFlag = 1;
				}
			}

			if (CarHitDetection()){
				break;
			}
			if (!(LM_UP_Pressed())){
				OpenSuccess = 1;
				break;
			}
		}
		Motor_Stop();
		ResetCurrentValue();
		LCD_DisplayCurrent(GetCurrentValue());
		ClearCarHitFlag();
		FAN_TurnOff(60000);
		ResetCounterTimer();
		LcdPrintString(0,0,"__");
		//LcdPrintString(2,0,"0.00S");
		if (OpenSuccess == 1){
			TotalCounter ++;
		}
		OpenSuccess = 0;
		LCD_DisplayCounter(TotalCounter);
	}
	//else if (DOWN_Button_Pressed()|| (SEN2_Pressed())){
	else if (DOWN_Button_Pressed()){
		LcdPrintString(0,0,"DW");
		FAN_TurnOn();
		dipSW23_value = (DIPSW_GetValue() & DIPSW23_MASK) >> DIPSW2_INDEX;
		switch (dipSW23_value){
			case 0:
				CloseDelayTimer = 1000;
				break;
			case 1:
				CloseDelayTimer = 3000;
				break;
			case 2:
				CloseDelayTimer = 0;
				break;
			case 3:
				CloseDelayTimer = 0;
				break;
			default:
				break;
		}
		VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
		ResetCounterTimer();
		while (!VTimerIsFired(VTimer_MotorDelayTimeout)){
			LCD_PrintTime(2,0,CloseDelayTimer - GetCounterTimer());
		}
		ObjectDetectFlag = 1;
		while (ObjectDetectFlag == 1){
			//UART_SendString("ObjectDetectFlag\r\n\t");
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
				if (SEN1_Pressed()){	// have no object in path // SEN1 = LOW
					ObjectDetectFlag = 0;
				}
			}
			else {
				if(!SEN1_Pressed()){	// have no object in path	// SEN1 = HIGH
					ObjectDetectFlag = 0;
				}
			}
		}

		//while (CloseSuccess == 0){
		Motor_Reverse();
		VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		ResetCounterTimer();
		LCD_PrintTime(2,0, MotorTotalTimer - GetCounterTimer());
		if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
			while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
				CalculateCurrentValue();
				if (calculate_done == 1){
					LCD_DisplayCurrent(GetCurrentValue());
					LCD_PrintTime(2,0, MotorTotalTimer  - GetCounterTimer());
					calculate_done = 0;
				}
				if (VTimerIsFired(VTimer_MotorDelayTimeout)){
					carhitDetectFlag = 1;
				}

				if (!LM_DOWN_Pressed()){
					CloseSuccess = 1;
					ClearCarHitFlag();
					break;
				}
				else {
					if(!SEN1_Pressed() || CarHitDetection()){	// Detect object
						ClearCarHitFlag();
						Motor_Forward();
						while (!(SEN1_Pressed())){
							if (!LM_UP_Pressed()){
								Motor_Stop();
							}
						}
						Motor_Reverse();	// resuming closing
						VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
						VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
						ResetCounterTimer();
					}
				}
			}
		}
		else {				// N/O
			//VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
			//ResetCounterTimer();
			//LCD_PrintTime(2,0, MotorTotalTimer - GetCounterTimer());
			while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
				CalculateCurrentValue();
				if (calculate_done == 1){
					LCD_PrintTime(2,0, MotorTotalTimer - GetCounterTimer());
					LCD_DisplayCurrent(GetCurrentValue());
					calculate_done = 0;
				}
				if (VTimerIsFired(VTimer_MotorDelayTimeout)){
					carhitDetectFlag = 1;
				}

				if (!LM_DOWN_Pressed()){
					CloseSuccess = 1;
					ClearCarHitFlag();
					break;
				}
				else {
					if(SEN1_Pressed()|| CarHitDetection()){	// Detect object
						Motor_Forward();
						ClearCarHitFlag();
						while ((SEN1_Pressed())){
							if (!LM_UP_Pressed()){
								Motor_Stop();
							}
						}
						Motor_Reverse();	// resuming closing
						VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
						VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
						ResetCounterTimer();
					}
				}
			}
		}
		Motor_Stop();
		FAN_TurnOff(60000);
		ResetCurrentValue();
		LCD_DisplayCurrent(GetCurrentValue());
		ClearCarHitFlag();
		ResetCounterTimer();
		LcdPrintString(0,0,"__");
		//LcdPrintString(2,0,"0.00S");
		ObjectDetectFlag = 0;
		if (CloseSuccess ==1){
			TotalCounter ++;
		}
		CloseSuccess = 0;
		LCD_DisplayCounter(TotalCounter);
		//GateState = GATE_CLOSED;
	}
}

void OpenGate(){
	uint8_t OpenSuccess = 0;
	if (UP_Button_Pressed()){
		LcdPrintString(0,0,"UP");
		//UART_SendString("UP_Button_Pressed\r\n\t");
		FAN_TurnOn();
		Motor_Forward();
		if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){
			MotorTotalTimer = 3000;
			LcdPrintString(5,1,"3S");
			//UART_SendString("MotorTotalTimer = 3000\r\n\t");
		}
		else {
			MotorTotalTimer = 6000;
			LcdPrintString(5,1,"6S");
			//UART_SendString("MotorTotalTimer = 6000\r\n\t");
		}
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		ResetCounterTimer();
		while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
			CalculateCurrentValue();
			if (calculate_done == 1){
				LCD_DisplayCurrent(GetCurrentValue());
				calculate_done = 0;
			}
			LCD_PrintTime(2,0, GetCounterTimer());
			if (CarHitDetection()){

			}
			if (!(LM_UP_Pressed())){
				//UART_SendString("OpenOK = 1\r\n\t");
				OpenSuccess = 1;
				break;
			}
		}
		Motor_Stop();
		/*if (OpenSuccess == 0) {
			VTimerSet(VTimer_MotorTotalTimeout,60000);
			while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
				LCD_PrintTime(2,0, GetCounterTimer());
			}
		}*/
		FAN_TurnOff(60000);
		ResetCounterTimer();
		LcdPrintString(2,0,"0.00S");
		//GateState = GATE_OPENED;
	}
}

void CloseGate(){
	uint8_t CloseSuccess = 0;
	uint8_t dipSW23_value = 0;
	uint8_t ObjectDetectFlag = 0;
	if (DOWN_Button_Pressed() || (!(SEN2_Pressed()))){
		LcdPrintString(0,0,"DW");
		//UART_SendString("DOWN_Button_Pressed\r\n\t");
		FAN_TurnOn();
		dipSW23_value = (DIPSW_GetValue() & DIPSW23_MASK) >> DIPSW2_INDEX;
		//LcdPrintNumber(0,1,dipSW23_value);
		switch (dipSW23_value){
			case 0:
				CloseDelayTimer = 1000;
				break;
			case 1:
				CloseDelayTimer = 3000;
				break;
			case 2:
				CloseDelayTimer = 1;
				break;
			case 3:
				CloseDelayTimer = 1;
				break;
			default:
				break;
		}
		VTimerSet(VTimer_MotorDelayTimeout,CloseDelayTimer);
		while (!VTimerIsFired(VTimer_MotorDelayTimeout));
		ObjectDetectFlag = 1;
		while (ObjectDetectFlag == 1){
			//UART_SendString("ObjectDetectFlag\r\n\t");
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
				if (SEN1_Pressed()){	// have no object in path // SEN1 = LOW
					ObjectDetectFlag = 0;
				}
			}
			else {
				if(!SEN1_Pressed()){	// have no object in path	// SEN1 = HIGH
					ObjectDetectFlag = 0;
				}
			}
		}

		while (CloseSuccess == 0){
			Motor_Reverse();
			VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
				if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
					MotorTotalTimer = 3000;
					LcdPrintString(5,1,"3S");
				}
				else {
					MotorTotalTimer = 6000;
					LcdPrintString(5,1,"6S");
				}
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				ResetCounterTimer();
				LCD_PrintTime(2,0, GetCounterTimer());
				while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
					CalculateCurrentValue();
					if (calculate_done == 1){
						LCD_DisplayCurrent(GetCurrentValue());
						calculate_done = 0;
					}
					if (VTimerIsFired(VTimer_MotorDelayTimeout)){
						carhitDetectFlag = 1;
					}
					LCD_PrintTime(2,0, GetCounterTimer());
					if (!LM_DOWN_Pressed()){
						CloseSuccess = 1;
						ClearCarHitFlag();
						break;
					}
					else {
						if(!SEN1_Pressed() || CarHitDetection()){	// Detect object
							ClearCarHitFlag();
							Motor_Forward();
							while (!(SEN1_Pressed())){
								if (!LM_UP_Pressed()){
									Motor_Stop();
								}
							}
							Motor_Reverse();	// resuming closing
							VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
							VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
							ResetCounterTimer();
						}
					}
				}
			}
			else {				// N/O
				if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
					MotorTotalTimer = 3000;
					LcdPrintString(5,1,"3S");
				}
				else {
					MotorTotalTimer = 6000;
					LcdPrintString(5,1,"6S");
				}
				VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
				ResetCounterTimer();
				LCD_PrintTime(2,0, GetCounterTimer());
				while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
					CalculateCurrentValue();
					if (calculate_done == 1){
						LCD_DisplayCurrent(GetCurrentValue());
						calculate_done = 0;
					}
					if (VTimerIsFired(VTimer_MotorDelayTimeout)){
						carhitDetectFlag = 1;
					}
					LCD_PrintTime(2,0, GetCounterTimer());
					if (!LM_DOWN_Pressed()){
						CloseSuccess = 1;
						break;
					}
					else {
						if(SEN1_Pressed()|| CarHitDetection()){	// Detect object
							Motor_Forward();
							ClearCarHitFlag();
							while ((SEN1_Pressed())){
								if (!LM_UP_Pressed()){
									Motor_Stop();
								}
							}
							Motor_Reverse();	// resuming closing
							VTimerSet(VTimer_MotorDelayTimeout,TIME_CHECK_CARHIT);
							VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
							ResetCounterTimer();
						}
					}
				}
			}
		}
		Motor_Stop();
		FAN_TurnOff(60000);
		ResetCounterTimer();
		//LcdPrintString(2,0,"0.00S");
		//GateState = GATE_CLOSED;
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
			UART_SendString("CAR HIT\r\n\t");
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
