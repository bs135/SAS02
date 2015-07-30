/*
 * EEPROM.c
 *
 *  Created on: Jul 30, 2015
 *      Author: XuanHung
 */

#include "EEPROM.h"

static I2C_XFER_T eeprom_xfer;
/* Data area for slave operations */
static uint8_t eeprom_data[I2C_SLAVE_EEPROM_SIZE + 1];
uint8_t buffer[I2C_SLAVE_EEPROM_SIZE];
void I2C_InitController(){
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_SFI2C_EN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_SFI2C_EN);

 	/* Initialize I2C */
 	Chip_I2C_Init(I2C0);

	Chip_I2C_SetClockRate(I2C0,I2C_SPEED_100KHZ);

 	Chip_I2C_SetMasterEventHandler(I2C0,Chip_I2C_EventHandler);
 	NVIC_EnableIRQ(I2C0_IRQn);

 }


/* Update the EEPROM state */
static void i2c_eeprom_update_state(I2C_XFER_T *xfer, uint8_t *buff, int sz)
{
	xfer->txBuff = xfer->rxBuff = &buff[buff[0] + 1];
	xfer->rxSz = xfer->txSz = sz - buff[0] + 1;
}
/* Slave event handler for simulated EEPROM */
static void I2C_EEPROM_Events(I2C_ID_T id, I2C_EVENT_T event)
{
	static int is_addr = 1;
	switch (event) {
	case I2C_EVENT_DONE:
		is_addr = 1;
		i2c_eeprom_update_state(&eeprom_xfer, eeprom_data, I2C_SLAVE_EEPROM_SIZE);
		eeprom_xfer.rxBuff = eeprom_data;
		eeprom_xfer.rxSz++;
		break;

	case I2C_EVENT_SLAVE_RX:
		if (is_addr) {
			is_addr = 0;
			eeprom_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);	/* Correct addr if required */
			i2c_eeprom_update_state(&eeprom_xfer, eeprom_data, I2C_SLAVE_EEPROM_SIZE);
			break;
		}

		eeprom_data[0]++;
		eeprom_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (eeprom_xfer.rxSz == 1) {
			i2c_eeprom_update_state(&eeprom_xfer, eeprom_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;

	case I2C_EVENT_SLAVE_TX:
		eeprom_data[0]++;
		eeprom_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (eeprom_xfer.txSz == 1) {
			i2c_eeprom_update_state(&eeprom_xfer, eeprom_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;
	}
}

void EEPROM_InitController(){
	I2C_InitController();
	memset(&eeprom_data[1], 0xFF, I2C_SLAVE_EEPROM_SIZE);
	eeprom_xfer.slaveAddr = (I2C_SLAVE_EEPROM_ADDR);
	eeprom_xfer.txBuff = &eeprom_data[1];
	eeprom_xfer.rxBuff = eeprom_data;
	eeprom_xfer.txSz = eeprom_xfer.rxSz = sizeof(eeprom_data);
	Chip_I2C_SlaveSetup(I2C0, I2C_SLAVE_0, &eeprom_xfer, I2C_EEPROM_Events, 0);
}

void EEPROM_ReadBytes(uint8_t address, uint8_t* data, uint8_t length){

	//take care of EEPROM address overflow
	eeprom_xfer.rxSz = length;
	eeprom_xfer.rxBuff = data;
	buffer[0] = address;
	eeprom_xfer.txBuff = buffer;
	Chip_I2C_MasterTransfer(I2C0,&eeprom_xfer);
	//DelayMs(10);
}

/*********************************************************************//**
 * @brief		Write a number of bytes to EEPROM
 * @param[in]	None
 * @note		Write function must delay 10ms  to wait EEPROM operate
 * @warning 	Be careful, you shouldn't call EEPROM write command from SysTick
 *				Handle because DelayMs() is a function use SysTick Handle to delay.
 * @return		None
 **********************************************************************/
void EEPROM_WriteBytes(uint8_t address, uint8_t* data, uint8_t length){
	uint8_t i;
	//take care of EEPROM address overflow
	eeprom_xfer.txSz = length + 1;
	buffer[0] = address & 0xFF;
	for(i = 0; i<length; i++){
		buffer[i + 1] = data[i];
	}
	eeprom_xfer.txBuff = buffer;
	//Chip_I2C_MasterSend(I2C0,eeprom_xfer.slaveAddr,eeprom_xfer.txBuff,eeprom_xfer.txSz);
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_EEPROM_ADDR << 1,buffer,length+1);
	//DelayMs(10);
}


