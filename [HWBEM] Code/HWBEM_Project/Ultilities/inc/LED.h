/*
 * LED.h
 *
 *  Created on: May 12, 2015
 *      Author: XuanHung
 */

#ifndef LED_H_
#define LED_H_

#include "chip.h"

#define LPC_GPIO_USER_LED   LPC_GPIO_PORT0_BASE
#define USER_LED_PORT		0
#define USER_LED_PIN		3

#define LPC_GPIO_DIRSW_LED	LPC_GPIO_PORT2_BASE
#define DIRSW_LED_PORT		0
#define UP_SW_LED_PIN		0
#define DW_SW_LED_PIN		1

#define LPC_GPIO_SEN_LED    LPC_GPIO_PORT2_BASE
#define SEN_LED_PORT		0
#define SEN1_LED_PIN		2
#define SEN2_LED_PIN		3

#define LPC_GPIO_LM_LED     LPC_GPIO_PORT1_BASE
#define LM_LED_PORT			0
#define UP_LM_LED_PIN		8
#define DW_LM_LED_PIN		11

void LED_InitController();

// User LED
void LED_TurnOnUserLED();
void LED_TurnOffUserLED();

// UP/DOWN Switch LED
void LED_TurnOnUPSWLED();
void LED_TurnOffUPSWLED();
void LED_TurnOnDWSWLED();
void LED_TurnOffDWSWLED();

// SEN LED
void LED_TurnOnSen1LED();
void LED_TurnOffSen1LED();
void LED_TurnOnSen2LED();
void LED_TurnOffSen2LED();

//  UP/DOWN limit LED
void LED_TurnOnUPLMLED();
void LED_TurnOffUPLMLED();
void LED_TurnOnDWLMLED();
void LED_TurnOffDWLMLED();

#endif /* LED_H_ */
