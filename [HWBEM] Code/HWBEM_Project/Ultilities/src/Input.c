/*
 * Input.c
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#include "Input.h"
#include "LED.h"

uint8_t InputValue[4] = {0,0,0,0};
uint8_t DipSWValue[4] = {0,0,0,0};
uint8_t SenValue[4] = {0,0,0,0};
uint8_t UpSwitchEdgeStatus = 0;
uint8_t UpSwitchLevel = HIGH_LEVEL;
uint8_t DownSwitchEdgeStatus = 0;
uint8_t DownSwitchLevel = HIGH_LEVEL;
uint8_t Switch3EdgeStatus = 0;
uint8_t Switch3Level = HIGH_LEVEL;
uint8_t Sen2SwitchEdgeStatus = 0;
uint8_t Sen2Level = LOW_LEVEL;
//uint8_t sen1Condition = NO_CONDITION;

void Input_InitController(){
	/* Configure GPIO pin as input pin */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTON_PORT, BUTTON_UP_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTON_PORT, BUTTON_DOWN_PIN);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SWITCH_PORT, SWITCH_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SWITCH3_PORT, SWITCH3_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_1, IOCON_MODE_INACT); // Switch3 pin input nopull
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SEN1_PORT, SEN1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SEN2_PORT, SEN2_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, IOCON_MODE_INACT); // Sen2 pin input nopull

	Chip_GPIO_SetPinDIRInput(LPC_GPIO, LM_UP_PORT, LM_UP_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, LM_DOWN_PORT, LM_DOWN_PIN);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_1_PORT, DIPSW_1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_2_PORT, DIPSW_2_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_3_PORT, DIPSW_3_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_4_PORT, DIPSW_4_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_5_PORT, DIPSW_5_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DIPSW_6_PORT, DIPSW_6_PIN);
}

uint8_t input_counter = 0;
uint8_t input_temp = 0;
uint8_t SEN2HoldFlag = 0;
void Input_Service(){
	if (input_counter > 5){
		// button and switch
		InputValue[3] = InputValue[2];
		InputValue[2] = InputValue[1];

		if (Chip_GPIO_GetPinState(LPC_GPIO,BUTTON_PORT,BUTTON_UP_PIN) == FALSE){ // UP SWITCH
			if (UpSwitchLevel == HIGH_LEVEL){
				UpSwitchEdgeStatus = FALLING_EDGE;
			}
			LED_TurnOnUPSWLED();
			InputValue[1] |= (1<<BUTTON_UP_INDEX);
		}
		else {
			if (UpSwitchLevel == LOW_LEVEL){
				UpSwitchEdgeStatus = RISING_EDGE;
			}
			LED_TurnOffUPSWLED();
			InputValue[1] &= ~(1<<BUTTON_UP_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,BUTTON_PORT,BUTTON_DOWN_PIN) == FALSE){	// DOWN SWITCH
			if (DownSwitchLevel == HIGH_LEVEL){
				DownSwitchEdgeStatus = FALLING_EDGE;
			}
			DownSwitchLevel = LOW_LEVEL;
			LED_TurnOnDWSWLED();
			InputValue[1] |= (1<<BUTTON_DOWN_INDEX);
		}
		else {
			if (DownSwitchLevel == LOW_LEVEL){
				DownSwitchEdgeStatus = RISING_EDGE;
			}
			DownSwitchLevel = HIGH_LEVEL;
			LED_TurnOffDWSWLED();
			InputValue[1] &= ~(1<<BUTTON_DOWN_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,SWITCH_PORT,SWITCH_PIN) == FALSE){		//SWITCH
			InputValue[1] |= (1<<SWITCH_INDEX);
		}
		else {
			InputValue[1] &= ~(1<<SWITCH_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,SWITCH3_PORT,SWITCH3_PIN) == FALSE){		//SWITCH 3
			if (Switch3Level == HIGH_LEVEL){
				Switch3EdgeStatus = FALLING_EDGE;
			}
			Switch3Level = LOW_LEVEL;
			InputValue[1] |= (1<<SWITCH3_INDEX);
		}
		else {
			if (Switch3Level == LOW_LEVEL){
				Switch3EdgeStatus = RISING_EDGE;
			}
			Switch3Level = HIGH_LEVEL;
			InputValue[1] &= ~(1<<SWITCH3_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,SEN1_PORT,SEN1_PIN) == FALSE){		// SEN1 = LOW
			if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_1_PORT) & (1<<DIPSW_1_PIN)) == 0){	// NC
				LED_TurnOffSen1LED();
			}
			else {
				LED_TurnOnSen1LED();
			}
			InputValue[1] |= (1<<SEN1_INDEX);
		}
		else {
			if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_1_PORT) & (1<<DIPSW_1_PIN)) == 0){	// NC
				LED_TurnOnSen1LED();
			}
			else {
				LED_TurnOffSen1LED();
			}
			InputValue[1] &= ~(1<<SEN1_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,SEN2_PORT,SEN2_PIN) == FALSE){ // SEN2 nguoc so voi cac input khac
			if (Sen2Level == HIGH_LEVEL){
				Sen2SwitchEdgeStatus = FALLING_EDGE;
			}
			Sen2Level = LOW_LEVEL;
			LED_TurnOffSen2LED();
			InputValue[1] &= ~(1<<SEN2_INDEX);
		}
		else {
			if (Sen2Level == LOW_LEVEL){
				Sen2SwitchEdgeStatus = RISING_EDGE;
			}
			Sen2Level = HIGH_LEVEL;
			LED_TurnOnSen2LED();
			InputValue[1] |= (1<<SEN2_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,LM_UP_PORT,LM_UP_PIN) == FALSE){
			LED_TurnOffUPLMLED();
			InputValue[1] &= ~(1<<LM_UP_INDEX);
		}
		else {
			LED_TurnOnUPLMLED();
			InputValue[1] |= (1<<LM_UP_INDEX);
		}

		if (Chip_GPIO_GetPinState(LPC_GPIO,LM_DOWN_PORT,LM_DOWN_PIN) == FALSE){
			LED_TurnOffDWLMLED();
			InputValue[1] &= ~(1<<LM_DOWN_INDEX);
		}
		else {
			LED_TurnOnDWLMLED();
			InputValue[1] |= (1<<LM_DOWN_INDEX);
		}

		if ((InputValue[3] == InputValue[2]) && (InputValue[2] == InputValue[1])){
			InputValue[0] = InputValue[1];
		}

		// dip switch
		DipSWValue[3] = DipSWValue[2];
		DipSWValue[2] = DipSWValue[1];
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_1_PORT) & (1<<DIPSW_1_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW1_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW1_INDEX);
		}
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_2_PORT) & (1<<DIPSW_2_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW2_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW2_INDEX);
		}
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_3_PORT) & (1<<DIPSW_3_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW3_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW3_INDEX);
		}
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_4_PORT) & (1<<DIPSW_4_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW4_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW4_INDEX);
		}
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_5_PORT) & (1<<DIPSW_5_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW5_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW5_INDEX);
		}
		if (( Chip_GPIO_GetPortValue(LPC_GPIO,DIPSW_6_PORT) & (1<<DIPSW_6_PIN)) == 0){
			DipSWValue[1] |= (1<<DIPSW6_INDEX);
		}
		else {
			DipSWValue[1] &= ~(1<<DIPSW6_INDEX);
		}
		if ((DipSWValue[3] == DipSWValue[2]) && (DipSWValue[2] == DipSWValue[1])){
			DipSWValue[0] = DipSWValue[1];
		}
		input_counter = 0;
	}
	else {
		input_counter++;
	}
}

uint8_t UP_Button_Pressed(){
	if ((InputValue[0] & (1<<BUTTON_UP_INDEX)) == (1<<BUTTON_UP_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t UP_GetEdgeStatus(){
	return UpSwitchEdgeStatus;
}
void UP_ClearEdgeStatus(){
	UpSwitchEdgeStatus = HIGH_NO_EDGE;
}

uint8_t DOWN_Button_Pressed(){
	if ((InputValue[0] & (1<<BUTTON_DOWN_INDEX)) == (1<<BUTTON_DOWN_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t DOWN_GetEdgeStatus(){
	return DownSwitchEdgeStatus;
}
void DOWN_ClearEdgeStatus(){
	DownSwitchEdgeStatus = HIGH_NO_EDGE;
}
uint8_t SWITCH_Pressed(){
	if (InputValue[0] & (1<<SWITCH_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t SWITCH3_Pressed(){
	if (InputValue[0] & (1<<SWITCH3_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t SWITCH3_GetEdgeStatus(){
	return Switch3EdgeStatus;
}
void SWITCH3_ClearEdgeStatus(){
	Switch3EdgeStatus = HIGH_NO_EDGE;
}
uint8_t SEN1_Pressed(){
	if ((InputValue[0] & (1<<SEN1_INDEX)) == (1<<SEN1_INDEX)){
		return TRUE;
	}
	else return FALSE;
}

uint8_t SEN2_Pressed(){
	if ((InputValue[0] & (1<<SEN2_INDEX)) == (1<<SEN2_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t SEN2_GetEdgeStatus(){
	return Sen2SwitchEdgeStatus;
}
void SEN2_ClearEdgeStatus(){
	Sen2SwitchEdgeStatus = HIGH_NO_EDGE;
}

uint8_t LM_UP_Pressed(){
	if ((InputValue[0] & (1<<LM_UP_INDEX)) ==  (1<<LM_UP_INDEX)){
		return TRUE;
	}
	else return FALSE;
}
uint8_t LM_DOWN_Pressed(){
	if ((InputValue[0] & (1<<LM_DOWN_INDEX)) == (1<<LM_DOWN_INDEX)){
		return TRUE;
	}
	else return FALSE;
}

uint8_t DIPSW_GetValue(){
	return DipSWValue[0];
}

