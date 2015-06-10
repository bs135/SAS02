/*
 * Input.h
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include "chip.h"

#define LPC_GPIO_BUTTON   	LPC_GPIO_PORT0_BASE
#define BUTTON_PORT			0
#define BUTTON_UP_PIN		2
#define BUTTON_DOWN_PIN		5

#define LPC_GPIO_SWITCH   	LPC_GPIO_PORT0_BASE
#define SWITCH_PORT			0
#define SWITCH_PIN			4

#define LPC_GPIO_SEN1   	LPC_GPIO_PORT0_BASE
#define SEN1_PORT			0
#define SEN1_PIN			6

#define LPC_GPIO_SEN2		LPC_GPIO_PORT0_BASE
#define SEN2_PORT			0
#define SEN2_PIN			7

#define LPC_GPIO_LM_UP	    LPC_GPIO_PORT3_BASE
#define LM_UP_PORT			3
#define LM_UP_PIN			4

#define LPC_GPIO_LM_DOWN    LPC_GPIO_PORT3_BASE
#define LM_DOWN_PORT		3
#define LM_DOWN_PIN			5

#define LPC_GPIO_DIPSW      LPC_GPIO_PORT3_BASE
#define DIPSW_PORT			3
#define DIPSW_1_PIN			0
#define DIPSW_2_PIN			1
#define DIPSW_3_PIN			2
#define DIPSW_4_PIN			3
//#define DIPSW_5_PIN			4
//#define DIPSW_6_PIN			5

void Input_InitController(void);

#endif /* INC_INPUT_H_ */
