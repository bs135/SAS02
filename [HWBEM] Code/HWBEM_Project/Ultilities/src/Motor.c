/*
 * Motor.c
 *
 *  Created on: May 14, 2015
 *      Author: XuanHung
 */

#include "Motor.h"
uint8_t VTimer_FanOffID;
uint8_t FanOffFlag = 0;
void Motor_InitController(){
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,MOTOR_PORT,MOTOR_FORWARD_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,MOTOR_PORT,MOTOR_REVERSE_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,FAN_PORT,FAN_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,FC2_PORT,FC2_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO,FC2_PORT,FC2_PIN);
	VTimer_FanOffID = VTimerGetID();
	FanOffFlag = 0;
}

void Motor_Forward(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO,MOTOR_PORT,MOTOR_FORWARD_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO,MOTOR_PORT,MOTOR_REVERSE_PIN);
}
void Motor_Reverse(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO,MOTOR_PORT,MOTOR_REVERSE_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO,MOTOR_PORT,MOTOR_FORWARD_PIN);
}
void Motor_Stop(){
	Chip_GPIO_SetPinOutLow(LPC_GPIO,MOTOR_PORT,MOTOR_FORWARD_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO,MOTOR_PORT,MOTOR_REVERSE_PIN);
}

void FAN_TurnOn(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO,FAN_PORT,FAN_PIN);
	FanOffFlag = 0;
}

void FAN_TurnOff(uint32_t DelayTime){
	VTimerSet(VTimer_FanOffID,DelayTime);
	FanOffFlag = 1;
}

void FanService(){
	if (FanOffFlag == 1){
		if (VTimerIsFired(VTimer_FanOffID)){
			Chip_GPIO_SetPinOutLow(LPC_GPIO,FAN_PORT,FAN_PIN);
			FanOffFlag = 0;
		}
	}
}
