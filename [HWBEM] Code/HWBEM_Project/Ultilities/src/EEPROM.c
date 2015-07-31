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

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;

volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t I2CSlaveState = I2C_IDLE;
volatile uint32_t timeout = 0;

volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint8_t I2CSlaveBuffer[BUFSIZE];
volatile uint32_t I2CCount = 0;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

#define	 I2C_TIMEOUT 100		//ms
volatile uint8_t I2CTimerID = 0;

void I2C_InitController(){

	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 4);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 5);
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

//void I2C_IRQHandler(void)
//{
//  uint8_t StatValue;
//
//  timeout = 0;
//  /* this handler deals with master read and master write only */
//  StatValue = LPC_I2C->STAT;
//  switch ( StatValue )
//  {
//	case 0x08:			/* A Start condition is issued. */
//	UART_SendString("START OK\r\n\t");
//	WrIndex = 0;
//	LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
//	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
//	break;
//
//	case 0x10:			/* A repeated started is issued */
//		UART_SendString("START RP\r\n\t");
//		RdIndex = 0;
//		/* Send SLA with R bit set, */
//		LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
//		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
//	break;
//
//	case 0x18:			/* Regardless, it's a ACK */
//
//	if ( I2CWriteLength == 1 )
//	{
//		UART_SendString("STOP\r\n\t");
//		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
//		I2CMasterState = I2C_NO_DATA;
//	}
//	else
//	{
//		UART_SendString("ADDRESS\r\n\t");
//	  LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
//	}
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
//	if ( WrIndex < I2CWriteLength )
//	{
//		UART_SendString("TEST6\r\n\t");
//	    LPC_I2C->DAT = I2CMasterBuffer[WrIndex++]; /* this should be the last one */
//	}
//	else
//	{
//	  if ( I2CReadLength != 0 )
//	  {
//		  UART_SendString("TEST7\r\n\t");
//		LPC_I2C->CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
//	  }
//	  else
//	  {
//		  UART_SendString("TEST8\r\n\t");
//		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
//		I2CMasterState = I2C_OK;
//	  }
//	}
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x30:
//		UART_SendString("TEST10\r\n\t");
//	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
//	I2CMasterState = I2C_NACK_ON_DATA;
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x40:	/* Master Receive, SLA_R has been sent */
//	if ( (RdIndex + 1) < I2CReadLength )
//	{
//		UART_SendString("TEST11\r\n\t");
//	  /* Will go to State 0x50 */
//	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
//	}
//	else
//	{
//	  /* Will go to State 0x58 */
//		UART_SendString("TEST12\r\n\t");
//	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK after data is received */
//	}
//	UART_SendString("TEST13\r\n\t");
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
//	I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
//	if ( (RdIndex + 1) < I2CReadLength )
//	{
//		UART_SendString("TEST14\r\n\t");
//	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
//	}
//	else
//	{
//		UART_SendString("TEST15\r\n\t");
//	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
//	}
//	UART_SendString("TEST16\r\n\t");
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x58:
//	I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
//	I2CMasterState = I2C_OK;
//	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
//	LPC_I2C->CONCLR = I2CONCLR_SIC;	/* Clear SI flag */
//	break;
//
//	case 0x20:		/* regardless, it's a NACK */
//	case 0x48:
//	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
//	I2CMasterState = I2C_NACK_ON_ADDRESS;
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//
//	case 0x38:		/* Arbitration lost, in this example, we don't
//					deal with multiple master situation */
//	default:
//	I2CMasterState = I2C_ARBITRATION_LOST;
//	LPC_I2C->CONCLR = I2CONCLR_SIC;
//	break;
//  }
//  return;
//}


void I2C_SendByte(uint8_t reg, uint8_t data){
	while (LPC_I2C->CONSET & I2CONSET_STO == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail1\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = EEPROM_ADDR;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	/*!< Send EEPROM address for write */
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_SLAW_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail2\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = reg;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail3\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = data;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail4\r\n\t");
			break;
		}
	}
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while (LPC_I2C->CONSET & I2CONSET_STO == 1);
	/*VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail5\r\n\t");
			break;
		}
	}*/
}

void I2C_ReadByte(uint8_t reg, uint8_t* data){
	while (LPC_I2C->CONSET & I2CONSET_STO == 1);
	/*--- Issue a start condition ---*/
	LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */
	/*--- Wait until START transmitted ---*/
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail1\r\n\t");
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
			UART_SendString("Fail2\r\n\t");
			break;
		}
	}
	LPC_I2C->DAT = reg ;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	LPC_I2C->CONCLR = (I2CONCLR_SIC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_DAT_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail3\r\n\t");
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
			UART_SendString("Fail4\r\n\t");
			break;
		}
	}
	/*!< Send EEPROM address for read */
	LPC_I2C->DAT = EEPROM_ADDR | RD_BIT;
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_SLAR_ACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail5\r\n\t");
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

	data[0] = LPC_I2C->DAT;
	LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_RX_DAT_NACK){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail7\r\n\t");
			break;
		}
	}
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	while (LPC_I2C->CONSET & I2CONSET_STO == 1);
	/*VTimerSet(I2CTimerID,I2C_TIMEOUT);
	while(LPC_I2C->STAT != I2C_I2STAT_M_TX_START){
		if(VTimerIsFired(I2CTimerID)) {
			UART_SendString("Fail8\r\n\t");
			break;
		}
	}*/
}

uint32_t I2CEngine( void )
{
  RdIndex = 0;
  WrIndex = 0;

  /*--- Issue a start condition ---*/
  LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */

  I2CMasterState = I2C_BUSY;

  while ( I2CMasterState == I2C_BUSY )
  {
	if ( timeout >= MAX_TIMEOUT )
	{
	  I2CMasterState = I2C_TIME_OUT;
	  break;
	}
	timeout++;
  }
  LPC_I2C->CONCLR = I2CONCLR_STAC;

  return ( I2CMasterState );
}

uint8_t EEPROM_ReadBytes(uint8_t address){
	I2CWriteLength = 3;
	I2CReadLength = 1;
	I2CMasterBuffer[0] = EEPROM_ADDR;
	I2CMasterBuffer[1] = address;		/* address */
	I2CMasterBuffer[2] = EEPROM_ADDR | RD_BIT;
	I2CEngine();
	return I2CMasterBuffer[3];
}

/*********************************************************************//**
 * @brief		Write a number of bytes to EEPROM
 * @param[in]	None
 * @note		Write function must delay 10ms  to wait EEPROM operate
 * @warning 	Be careful, you shouldn't call EEPROM write command from SysTick
 *				Handle because DelayMs() is a function use SysTick Handle to delay.
 * @return		None
 **********************************************************************/
void EEPROM_WriteBytes(uint8_t address, uint8_t data){
	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = EEPROM_ADDR;
	I2CMasterBuffer[1] = address;		/* address */
	I2CMasterBuffer[2] = data;		/* Data0 */
	//I2CMasterBuffer[3] = 0xAA;		/* Data1 */
	//I2CMasterBuffer[4] = 0x12;		/* Data0 */
	//I2CMasterBuffer[5] = 0x34;		/* Data1 */
	I2CEngine();
}



