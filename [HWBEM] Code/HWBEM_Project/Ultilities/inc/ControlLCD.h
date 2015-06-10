#ifndef _LCD_H
#define _LCD_H

#include "chip.h"

/*
PIN RS : 	1:D0-D7 la command
0:D0-D7 la data
PIN R/W	1:READ
0:WRITE
PIN E		1:ENABLE
0:DISABLE
PIN E tu 1-> 0: Truyen data/command xuong LCD
*/
#define LPC_GPIO_LCD_CTRL   LPC_GPIO_PORT2_BASE
#define LCD_CTRL_PORT		0
#define LCD_RS_PIN			8
#define LCD_RD_PIN			9
#define LCD_EN_PIN			10

#define LPC_GPIO_LCD_DATA   LPC_GPIO_PORT2_BASE
#define LCD_DATA_PORT		0
#define LCD_D4_PIN			4
#define LCD_D5_PIN			5
#define LCD_D6_PIN			6
#define LCD_D7_PIN			7

#define LPC_GPIO_LCD_BKL   	LPC_GPIO_PORT1_BASE
#define LCD_BKL_PORT		0
#define LCD_BKL_PIN			9

#define READ_MODE			1
#define WRITE_MODE			0

#define COMMAND_MODE		0
#define DATA_MODE			1

#define ENABLE				1
#define DISABLE				0

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_CURSORSHIFT 0x10
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/******************************************************************************/
/* LCD_4bit.c: Functions for 2 line 16 character LCD, with 4-bit interface    */
/******************************************************************************/
uint8_t LCD_WaitBusy(void);
void LCD_Write4Bit(uint32_t data);
void LCD_WriteCommand(uint32_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_PutDigi3(uint8_t X, uint8_t Y,int num);
void LCD_PutDigi4(uint8_t X, uint8_t Y,int num);
void LCD_PutDigi5(uint8_t X, uint8_t Y,int num);
void LCD_PutBinary(uint8_t X, uint8_t Y,uint16_t num);
void LCD_InitController();
void LCD_GotoXY(uint8_t x, uint8_t y);
void LCDScrollLeft();
void LCDScrollRight();
void LCD_DataOut(uint8_t val);
void LCD_Clear(void);
void LCD_PrintString(uint8_t col,uint8_t row,char *str);
void LCD_GotoXY(uint8_t  sx,uint8_t sy );
void LCD_PrintChar (uint8_t row, uint8_t column, uint8_t val);
void LCD_PrintNumber (uint8_t x, uint8_t y, int number, uint8_t quantity);
void LCD_PrintString (uint8_t x, uint8_t y, char * ptr);
void LCD_Delay(uint32_t cnt);

void LCD_SetDataPortOutput();
void LCD_SetDataPortInput();
void LCD_SetDataMode();
void LCD_SetCommandMode();
void LCD_SetReadMode();
void LCD_SetWriteMode();
void LCD_EN_PIN_HIGH();
void LCD_EN_PIN_LOW();
void DelayMs(uint16_t a);
#endif





















 
