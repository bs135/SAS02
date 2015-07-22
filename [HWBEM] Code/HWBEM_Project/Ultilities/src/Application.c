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

uint32_t MotorTotalTimer = 0;
uint32_t CloseDelayTimer = 0;
uint8_t VTimer_MotorTotalTimeout;
uint8_t VTimer_MotorDelayTimeout;
uint8_t GateState = 0;

uint32_t CounterTimer = 0;

void System_Init(){
	VTimer_MotorTotalTimeout = VTimerGetID();
	VTimer_MotorDelayTimeout = VTimerGetID();
	LCD_Clear();
	LcdPrintString(0,0,"__");
	if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
		LcdPrintString(11,1,"sNC");
	}
	else {
		LcdPrintString(11,1,"sNO");
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
		while (LM_DOWN_Pressed()){
			FAN_TurnOn();
			Motor_Reverse();
			if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){	// N/C
				if(!SEN1_Pressed()){	// Detect object
					Motor_Forward();
					while (LM_UP_Pressed()){
					}
					Motor_Stop();
					GateState = GATE_OPENED;
					break;
				}
			}
			else {
				if(SEN1_Pressed()){	// Detect object
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
	FAN_TurnOff(1);
	Motor_Stop();
//	UART_SendString("GATE_CLOSED\r\n\t");
	LcdPrintString(2,0,"0.00S");
	if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){
		LcdPrintString(5,1,"3S");
	}
	else {
		LcdPrintString(5,1,"6S");
	}

/*	if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
		LcdPrintString(11,1,"sNC");
	}
	else {
		LcdPrintString(11,1,"sNO");
	}*/
}

void System_Running(){
	if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// DIPSW1 = ON , SEN1 = N/C
		LcdPrintString(11,1,"sNC");
	}
	else {
		LcdPrintString(11,1,"sNO");
	}
	if (GateState == GATE_CLOSED){
		OpenGate();
	}
	else if (GateState == GATE_OPENED){
		CloseGate();
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
			LCD_PrintTime(2,0, GetCounterTimer());
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
		GateState = GATE_OPENED;
	}
}

void CloseGate(){
	uint8_t CloseSuccess = 0;
	uint8_t dipSW23_value = 0;
	uint8_t ObjectDetectFlag = 0;
	if (DOWN_Button_Pressed()){
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
					LCD_PrintTime(2,0, GetCounterTimer());
					if (!LM_DOWN_Pressed()){
						CloseSuccess = 1;
						break;
					}
					else {
						if(!SEN1_Pressed()){	// Detect object
							Motor_Forward();
							while (!(SEN1_Pressed())){
								if (!LM_UP_Pressed()){
									Motor_Stop();
								}
							}
							Motor_Reverse();	// resuming closing
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
					LCD_PrintTime(2,0, GetCounterTimer());
					if (!LM_DOWN_Pressed()){
						CloseSuccess = 1;
						break;
					}
					else {
						if(SEN1_Pressed()){	// Detect object
							Motor_Forward();
							while ((SEN1_Pressed())){
								if (!LM_UP_Pressed()){
									Motor_Stop();
								}
							}
							Motor_Reverse();	// resuming closing
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
		LcdPrintString(2,0,"0.00S");
		GateState = GATE_CLOSED;
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
