/*
 * Input.c
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#include "Input.h"

void Input_InitController(){
	/* Configure GPIO pin as input pin */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_BUTTON, BUTTON_PORT, BUTTON_UP_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_BUTTON, BUTTON_PORT, BUTTON_DOWN_PIN);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_SWITCH, SWITCH_PORT, SWITCH_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_SEN1, SEN1_PORT, SEN1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_SEN2, SEN2_PORT, SEN2_PIN);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_LM_UP, LM_UP_PORT, LM_UP_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_LM_DOWN, LM_DOWN_PORT, LM_DOWN_PIN);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_DIPSW, DIPSW_PORT, DIPSW_1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_DIPSW, DIPSW_PORT, DIPSW_2_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_DIPSW, DIPSW_PORT, DIPSW_3_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_DIPSW, DIPSW_PORT, DIPSW_4_PIN);
}

void Input_Service(){

}

uint8_t DIPSW_GetValue(){

}
