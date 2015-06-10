/*
 * UART.c
 *
 *  Created on: Jun 8, 2015
 *      Author: XuanHung
 */
#include "UART.h"

STATIC RINGBUFF_T rxring;
/* Receive ring buffer sizes */
#define UART_RRB_SIZE 128	/* Receive */
/* Receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE];

void UART_InitController(){
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));/* RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));/* TXD */
	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);

	/* Before using the ring buffers, initialize them using the ring
		   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);


}

void UART_SendByte(uint8_t _data){
	Chip_UART_SendByte(LPC_USART,_data);
}

void UART_SendString(const char *_data){
	while(*_data != '\t')
	{
		UART_SendByte(*_data);
		_data++;
	}
}

void UART_SendNumber(uint16_t _number){
	if (_number >= 10000) { 	// 5 char
		UART_SendByte((_number/10000) + '0' );
		UART_SendByte(((_number/1000) % 10 )+ '0' );
		UART_SendByte(((_number/100) % 10 )+ '0' );
		UART_SendByte(((_number/10) % 10 )+ '0' );
		UART_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 1000) { 	// 4 char
		UART_SendByte(((_number/1000))+ '0' );
		UART_SendByte(((_number/100) % 10 )+ '0' );
		UART_SendByte(((_number/10) % 10 )+ '0' );
		UART_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 100) {	 // 3 char
		UART_SendByte(((_number/100) )+ '0' );
		UART_SendByte(((_number/10) % 10 )+ '0' );
		UART_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 10) { 	// 2 char
		UART_SendByte(((_number/10))+ '0' );
		UART_SendByte((_number % 10 )+ '0' );
	}
	else {					// 1 char
		UART_SendByte(_number+ '0' );
	}
}

void UART_IRQHandler(void)
{
	Chip_UART_RXIntHandlerRB(LPC_USART,&rxring);
}


uint8_t UART_ReceiveByte(){
	uint8_t _data;
	Chip_UART_ReadRB(LPC_USART, &rxring, &_data, 1);
	return _data;
}

uint8_t UART_RXAvailable(){
	return 	RingBuffer_GetCount(&rxring);
}
