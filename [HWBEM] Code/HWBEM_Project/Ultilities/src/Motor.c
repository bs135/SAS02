/*
 * Motor.c
 *
 *  Created on: May 14, 2015
 *      Author: XuanHung
 */

#include "Motor.h"

void Motor_InitController(){
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,MOTOR_PORT,MOTOR_FORWARD_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,MOTOR_PORT,MOTOR_REVERSE_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,FAN_PORT,FAN_PIN);
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
	Chip_GPIO_SetPinOutLow(LPC_GPIO,FAN_PORT,FAN_PIN);
}

void FAN_TurnOff(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO,FAN_PORT,FAN_PIN);
}
