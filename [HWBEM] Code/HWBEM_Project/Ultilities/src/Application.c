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
void System_Init(){
	VTimer_MotorTotalTimeout = VTimerGetID();
	VTimer_MotorDelayTimeout = VTimerGetID();
}

void System_Running(){
	if (GateState == GATE_CLOSED){
		OpenGate();
	}
	else {
		CloseGate();
	}

}

void OpenGate(){
	uint8_t OpenFail = 0;
	if (UP_Button_Pressed()){
		UART_SendString("UP_Button_Pressed\r\n\t");
		FAN_TurnOn();
		Motor_Forward();
		if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){
			MotorTotalTimer = 3000;
			UART_SendString("MotorTotalTimer = 3000\r\n\t");
		}
		else {
			MotorTotalTimer = 6000;
			UART_SendString("MotorTotalTimer = 6000\r\n\t");
		}
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
			if (!(LM_UP_Pressed())){
				UART_SendString("OpenFail = 1\r\n\t");
				OpenFail = 1;
				break;
			}
		}
		Motor_Stop();
		if (OpenFail == 0) {
			UART_SendString("wait = 1\r\n\t");
			DelayMs(60000);
		}
		FAN_TurnOff();
	}
	GateState == GATE_OPENED;
}

void CloseGate(){
	uint8_t dipSW23_value = 0;
	uint8_t sen1_detect_flag = 0;
	if (DOWN_Button_Pressed()){
		UART_SendString("UP_Button_Pressed\r\n\t");
		FAN_TurnOn();
		dipSW23_value = (DIPSW_GetValue() & DIPSW23_MASK) >> DIPSW2_INDEX;
		LcdPrintNumber(0,1,dipSW23_value);
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
		//DelayMs(CloseDelayTimer);
		while (!VTimerIsFired(VTimer_MotorDelayTimeout));
		while (sen1_detect_flag == 0){
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// N/C
				if (SEN1_Pressed()){
					Motor_Reverse();
					sen1_detect_flag = 1;
				}
			}
			else {	// N/O
				if(!SEN1_Pressed()){
					Motor_Reverse();
					sen1_detect_flag = 1;
				}
			}
		}
		LcdPrintString(0,1,"Tested");
		if ((DIPSW_GetValue() & (1<<DIPSW4_INDEX)) == (1<<DIPSW4_INDEX)){
			MotorTotalTimer = 3000;
			UART_SendString("MotorTotalTimer = 3000\r\n\t");
		}
		else {
			MotorTotalTimer = 6000;
			UART_SendString("MotorTotalTimer = 6000\r\n\t");
		}
		VTimerSet(VTimer_MotorTotalTimeout,MotorTotalTimer);
		while (!VTimerIsFired(VTimer_MotorTotalTimeout)){
			if ((DIPSW_GetValue() & (1<<DIPSW1_INDEX)) == (1<<DIPSW1_INDEX)){	// N/C
				if (SEN1_Pressed()){
					Motor_Forward();
					sen1_detect_flag = 1;
				}
			}
			else {	// N/O
				if(!SEN1_Pressed()){
					Motor_Forward();
					sen1_detect_flag = 1;
				}
			}
		}
	}

}
