/*
 * EEPROM.c
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#include "EEPROM.h"

void EEPROM_InitController(){
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1);

	/* Initialize I2C */
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0,I2C_DEFAULT_SPEED);

	Chip_I2C_SetMasterEventHandler(I2C0,Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

}
