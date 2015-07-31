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

#define BUFSIZE             64
#define MAX_TIMEOUT         0x00FFFFFF

#define I2CMASTER           0x01
#define I2CSLAVE            0x02

#define EEPROM_ADDR        	0xA0
#define READ_WRITE          0x01

#define RD_BIT              0x01

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C           0x00000000  /* I2C Data Reg */
#define I2ADR_I2C           0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH         0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH		0x00000015  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL		0x00000015  /* Fast Plus I2C SCL Duty Cycle Low Reg */

void I2C_InitController();
void I2C_SendByte(uint8_t address, uint8_t data);
void I2C_SendBytes(uint8_t address, uint8_t* data,uint8_t length);
void I2C_ReadByte(uint8_t reg, uint8_t* data);
void I2C_ReadBytes(uint8_t address, uint8_t* data,uint8_t length);
uint8_t EEPROM_ReadByte(uint8_t address);
void EEPROM_ReadBytes(uint8_t address,uint8_t* data,uint8_t length);
void EEPROM_WriteByte(uint8_t address, uint8_t data);
void EEPROM_WriteBytes(uint8_t address, uint8_t* data,uint8_t length);
#endif /* INC_EEPROM_H_ */

