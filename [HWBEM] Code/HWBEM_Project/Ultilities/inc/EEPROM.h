/*
 * EEPROM.h
 *
 *  Created on: Jun 11, 2015
 *      Author: XuanHung
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#include "chip.h"

#define EEPROM_I2C          	I2C0

#define I2C_SPEED_100KHZ        100000
#define I2C_SPEED_400KHZ        400000
#define I2C_DEFAULT_SPEED    	I2C_SPEED_400KHZ

/* EEPROM SLAVE data */
#define I2C_SLAVE_EEPROM_SIZE       64
#define I2C_SLAVE_EEPROM_ADDR       0x50

void EEPROM_InitController(void);

#endif /* INC_EEPROM_H_ */
