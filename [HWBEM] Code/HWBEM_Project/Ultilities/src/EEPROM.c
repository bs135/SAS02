/*
 * EEPROM.c
 *
 *  Created on: Jul 30, 2015
 *      Author: XuanHung
 */

#include "EEPROM.h"
#include "UART.h"
#include "chip.h"
#include "VTimer.h"

#define	 I2C_TIMEOUT 100		//ms
volatile uint8_t I2CTimerID = 0;

void I2C_InitController(){

	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1);
	Chip_I2C_SetClockRate(I2C0,I2C_SPEED_100KHZ);
 	Chip_I2C_Init(I2C0);
 	LPC_I2C->SCLL   = I2SCLL_SCLL;
 	LPC_I2C->SCLH   = I2SCLH_SCLH;
 	LPC_I2C->CONSET = I2CONSET_I2EN;
 //	NVIC_EnableIRQ(I2C0_IRQn);
 	I2CTimerID = VTimerGetID();

}

void I2C_SendByte(uint8_t address, uint8_t data){
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail1\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = EEPROM_ADDR;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	/*!< Send EEPROM address for write */
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_SLAW_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			///UART_SendString("Fail2\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = address;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail3\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = data;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail4\r\n\t");
			break;
		}
	}
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
}


void I2C_SendBytes(uint8_t address, uint8_t* data,uint8_t length){
	uint8_t index = 0;
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail1\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = EEPROM_ADDR;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	/*!< Send EEPROM address for write */
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_SLAW_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail2\r\n\t");
			break;
		}
	}

	LPC_I2C->DAT = address;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail3\r\n\t");
			break;
		}
	}

	for (index = 0;index < length;index++){
		LPC_I2C->DAT = data[index];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		VTimerSet(I2CTimerID,I2C_TIMEOUT);
		while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
			if(VTimerIsFired(I2CTimerID)) {
				UART_SendString("Fail4\r\n\t");
				break;
			}
		}
	}

	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
}

void I2C_ReadByte(uint8_t reg, uint8_t* data){
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail1\r\n\t");
			break;
		}
	}
	/*!< Send EEPROM address for write */
	LPC_I2C->DAT = EEPROM_ADDR;
	//LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_SLAW_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail2\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = reg ;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail3\r\n\t");
			break;
		}
	}
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	/*!< Send START condition a second time */
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_RESTART){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail4\r\n\t");
			break;
		}
	}
	/*!< Send EEPROM address for read */
	LPC_I2C->DAT = EEPROM_ADDR | RD_BIT;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_SLAR_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail5\r\n\t");
			break;
		}
	}
	LPC_I2C->CONSET = I2CONSET_AA;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail6\r\n\t");
			break;
		}
	}
	data[0] = LPC_I2C->DAT;
	LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_NACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail7\r\n\t");
			break;
		}
	}

	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
}


void I2C_ReadBytes(uint8_t address, uint8_t* data,uint8_t length){
	uint8_t index;
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail1\r\n\t");
			break;
		}
	}
	/*!< Send EEPROM address for write */
	LPC_I2C->DAT = EEPROM_ADDR;
	//LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_SLAW_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail2\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = address ;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail3\r\n\t");
			break;
		}
	}
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	/*!< Send START condition a second time */
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_RESTART){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail4\r\n\t");
			break;
		}
	}
	/*!< Send EEPROM address for read */
	LPC_I2C->DAT = EEPROM_ADDR | RD_BIT;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_SLAR_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail5\r\n\t");
			break;
		}
	}

	LPC_I2C->CONSET = I2CONSET_AA;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail6\r\n\t");
			break;
		}
	}
	for (index = 0;index < (length - 1);index++){
		data[index] = LPC_I2C->DAT;
		//LPC_I2C->CONCLR = I2CONCLR_AAC;
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		VTimerSet(I2CTimerID,I2C_TIMEOUT);
		while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_ACK){
			if(VTimerIsFired(I2CTimerID)) {
				UART_SendString("Fail7\r\n\t");
				break;
			}
		}
	}
	data[index] = LPC_I2C->DAT;
	LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_NACK){
		if(VTimerIsFired(I2CTimerID)) {
			//UART_SendString("Fail7\r\n\t");
			break;
		}
	}
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while ((LPC_I2C->CONSET & I2CONSET_STO) == 1);
}

uint8_t EEPROM_ReadByte(uint8_t address){
	uint8_t data;
	I2C_ReadByte(address,&data);
	return data;
}
void EEPROM_ReadBytes(uint8_t address,uint8_t* data,uint8_t length){
	I2C_ReadBytes(address,data,length);
}

void EEPROM_WriteByte(uint8_t address, uint8_t data){
	I2C_SendByte(address,data);
}
void EEPROM_WriteBytes(uint8_t address, uint8_t* data,uint8_t length){
	I2C_SendBytes(address,data,length);
}


