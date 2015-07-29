/*
 * EEPROM.c
 *
 *  Created on: Jul 30, 2015
 *      Author: XuanHung
 */

#include "EEPROM.h"

void EEPROM_InitController(){
static I2C_XFER_T eeprom_xfer;
/* Data area for slave operations */
static uint8_t eeprom_data[I2C_SLAVE_EEPROM_SIZE + 1];

void I2C_InitController(){
	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_SFI2C_EN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_SFI2C_EN);

 	/* Initialize I2C */
 	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0,I2C_DEFAULT_SPEED);
	Chip_I2C_SetClockRate(I2C0,I2C_SPEED_400KHZ);

 	Chip_I2C_SetMasterEventHandler(I2C0,Chip_I2C_EventHandler);
 	NVIC_EnableIRQ(I2C0_IRQn);

 }

void EEPROM_InitController(){
	I2C_InitController();
	memset(&eeprom_data[1], 0xFF, I2C_SLAVE_EEPROM_SIZE);
	eeprom_xfer.slaveAddr = (I2C_SLAVE_EEPROM_ADDR << 1);
	eeprom_xfer.txBuff = &eeprom_data[1];
	eeprom_xfer.rxBuff = eeprom_data;
	eeprom_xfer.txSz = seep_xfer.rxSz = sizeof(eeprom_data);
	Chip_I2C_SlaveSetup(I2C0, I2C_SLAVE_0, &eeprom_xfer, I2C_EEPROM_Events, 0);
}



/* Slave event handler for simulated EEPROM */
static void I2C_EEPROM_Events(I2C_ID_T id, I2C_EVENT_T event)
{
	static int is_addr = 1;
	switch (event) {
	case I2C_EVENT_DONE:
		is_addr = 1;
		i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		seep_xfer.rxBuff = seep_data;
		seep_xfer.rxSz++;
		break;

	case I2C_EVENT_SLAVE_RX:
		if (is_addr) {
			is_addr = 0;
			seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);	/* Correct addr if required */
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
			break;
		}

		seep_data[0]++;
		seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (seep_xfer.rxSz == 1) {
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;

	case I2C_EVENT_SLAVE_TX:
		seep_data[0]++;
		seep_data[0] &= (I2C_SLAVE_EEPROM_SIZE - 1);
		if (seep_xfer.txSz == 1) {
			i2c_eeprom_update_state(&seep_xfer, seep_data, I2C_SLAVE_EEPROM_SIZE);
		}
		break;
	}
}

void EEPROM_ReadBytes(uint8_t address, uint8_t* data, uint8_t length){
	uint8_t memory_addr[2];

	//take care of EEPROM address overflow
	memory_addr[0] = (address >> 8) & 0xFF;					//high byte
	memory_addr[1] = address & 0xFF;							//low byte
	I2C_Send2Receive(EEPROM_PORT, EEPROM_ADDR, memory_addr, 2, data, length, BLOCKING);
	DelayMs(10);
}

/*********************************************************************//**
 * @brief		Write a number of bytes to EEPROM
 * @param[in]	None
 * @note		Write function must delay 10ms  to wait EEPROM operate
 * @warning 	Be careful, you shouldn't call EEPROM write command from SysTick
 *				Handle because DelayMs() is a function use SysTick Handle to delay.
 * @return		None
 **********************************************************************/
void EEPROM_WriteBytes(uint16_t address, uint8_t* data, uint8_t length){
	uint8_t data_array[MAXIMUM_LENGTH_BYTES+2];
	uint8_t i;
	if (length > MAXIMUM_LENGTH_BYTES) length = MAXIMUM_LENGTH_BYTES;
	//take care of EEPROM address overflow
	data_array[0] = (address >> 8) & 0xFF;					//high byte
	data_array[1] = address & 0xFF;						//low byte
for(i = 0; i<length; i++)
	{
		data_array[i+2] = data[i];
	}
	I2C_Send(EEPROM_PORT,EEPROM_ADDR, data_array, length+2, BLOCKING);
	DelayMs(10);
}
