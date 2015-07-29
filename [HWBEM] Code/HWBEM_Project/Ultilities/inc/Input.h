/*
 * Input.h
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include "chip.h"

#define BUTTON_PORT			1
#define BUTTON_UP_PIN		2
#define BUTTON_DOWN_PIN		5

#define SWITCH_PORT			1
#define SWITCH_PIN			4

#define SEN1_PORT			0
#define SEN1_PIN			6

#define SEN2_PORT			0
#define SEN2_PIN			7

#define LM_UP_PORT			3
#define LM_UP_PIN			4

#define LM_DOWN_PORT		3
#define LM_DOWN_PIN			5

#define BUTTON_UP_INDEX			0
#define BUTTON_DOWN_INDEX		1
#define SWITCH_INDEX			2
#define SEN1_INDEX				3
#define SEN2_INDEX				4
#define LM_UP_INDEX				5
#define LM_DOWN_INDEX			6

#define DIPSW_1_PORT		3
#define DIPSW_1_PIN			0
#define DIPSW_2_PORT		3
#define DIPSW_2_PIN			1
#define DIPSW_3_PORT		3
#define DIPSW_3_PIN			2
#define DIPSW_4_PORT		3
#define DIPSW_4_PIN			3
#define DIPSW_5_PORT		0
#define DIPSW_5_PIN			11
#define DIPSW_6_PORT		0
#define DIPSW_6_PIN			2

#define DIPSW1_INDEX		0
#define DIPSW2_INDEX		1
#define DIPSW3_INDEX		2
#define DIPSW4_INDEX		3
#define DIPSW5_INDEX		4
#define DIPSW6_INDEX		5

#define DIPSW23_MASK		0x06

void Input_InitController(void);

uint8_t UP_Button_Pressed(void);
uint8_t DOWN_Button_Pressed(void);
uint8_t SWITCH_Pressed(void);
uint8_t SEN1_Pressed(void);
uint8_t SEN2_Pressed(void);
uint8_t LM_UP_Pressed(void);
uint8_t LM_DOWN_Pressed(void);

void Input_Service(void);
uint8_t DIPSW_GetValue();

#endif /* INC_INPUT_H_ */
