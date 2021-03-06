/*
 * Motor.h
 *
 *  Created on: May 14, 2015
 *      Author: XuanHung
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "chip.h"

#define LPC_GPIO_MOTOR    	LPC_GPIO_PORT0_BASE
#define MOTOR_PORT			0
#define MOTOR_FORWARD_PIN	8
#define MOTOR_REVERSE_PIN	9


#define LPC_GPIO_FAN     	LPC_GPIO_PORT1_BASE
#define FAN_PORT			0
#define FAN_PIN				10

void Motor_InitController();
void Motor_Forward();
void Motor_Reverse();
void FAN_TurnOn();
void FAN_TurnOff();

#endif /* MOTOR_H_ */
