/*
 * UART.h
 *
 *  Created on: Jun 8, 2015
 *      Author: XuanHung
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "chip.h"

void UART_InitController();
void UART_SendByte(uint8_t _data);
void UART_SendString(uint8_t *_data);
void UART_SendNumber(uint16_t _number);
void UART_ISR(void);
uint8_t UART_ReceiveByte();
uint8_t UART_RXAvailable();

#endif /* INC_UART_H_ */
