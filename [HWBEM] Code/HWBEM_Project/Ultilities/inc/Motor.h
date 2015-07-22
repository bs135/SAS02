/*
 * Motor.h
 *
 *  Created on: May 14, 2015
 *      Author: XuanHung
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "chip.h"

#define LPC_GPIO_MOTOR    	LPC_GPIO
#define MOTOR_PORT			0
#define MOTOR_FORWARD_PIN	8
#define MOTOR_REVERSE_PIN	9


#define LPC_GPIO_FAN     	LPC_GPIO
#define FAN_PORT			1
#define FAN_PIN				10

void Motor_InitController();
void Motor_Forward();
void Motor_Reverse();
void Motor_Stop();
void FAN_TurnOn();
void FAN_TurnOff(uint32_t);
void FanService();
#endif /* MOTOR_H_ */
