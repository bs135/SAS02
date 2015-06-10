#include "LCD.h"
#include "VTimer.h"

void LCD_InitController(){
	/* Set all command pins for LCD as outputs    */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RS_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RD_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_EN_PIN);
	/* Set all data pins for LCD as outputs    */
	LCD_SetDataPortOutput();
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,LCD_D4_PIN);
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,LCD_D5_PIN);
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,LCD_D6_PIN);
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,LCD_D7_PIN);
	/* Set backlight pins for LCD as outputs    */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_LCD_BKL,LCD_BKL_PORT,LCD_BKL_PIN);
	
	LCD_EN_PIN_LOW();
	LCD_SetWriteMode();
	LCD_SetCommandMode();
	DelayMs(16);			//wait for more than 15ms
	LCD_Write4Bit(0x03);    /* Select 4-bit interface             */
	DelayMs(16);			//wait for more than 15ms
	LCD_Write4Bit(0x03);
	DelayMs(16);			//wait for more than 15ms
	LCD_Write4Bit(0x03);  
	DelayMs(16);			//wait for more than 15ms
	LCD_Write4Bit(0x02);	//Start operation in 4-bit mode
	DelayMs(16);			//wait for more than 15ms
	LCD_WriteCommand(0x28);  /* 2 lines, 5x7 character matrix      */
	DelayMs(16);			//wait for more than 15ms
	LCD_WriteCommand(0x0C);  /* Display ctrl:Disp=ON,Curs/Blnk=ON */
	DelayMs(16);			//wait for more than 15ms
	LCD_WriteCommand(0x00);  /* Entry mode: Move right, no shift   */
	DelayMs(16);			//wait for more than 15ms
		   
	LCD_Clear();
}

void LCD_SetDataPortOutput(){
	Chip_GPIO_SetPortDIROutput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,0x000F<<LCD_D4_PIN);
}
void LCD_SetDataPortInput(){
	Chip_GPIO_SetPortDIRInput(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,0x000F<<LCD_D4_PIN);
}
void LCD_SetDataMode(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RS_PIN);
}
void LCD_SetCommandMode(){
	Chip_GPIO_SetPinOutLow(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RS_PIN);
}
void LCD_SetReadMode(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RD_PIN);
}
void LCD_SetWriteMode(){
	Chip_GPIO_SetPinOutLow(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_RD_PIN);
}
void LCD_EN_PIN_LOW(){
	Chip_GPIO_SetPinOutLow(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_EN_PIN);
}
void LCD_EN_PIN_HIGH(){
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_LCD_CTRL,LCD_CTRL_PORT,LCD_EN_PIN);
}

void LCD_Write4Bit(uint32_t data){
	LCD_SetWriteMode();	//keo chan RW xuong 0
	Chip_GPIO_SetPortOutHigh(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,(data&0x000F)<<LCD_D4_PIN);
	Chip_GPIO_SetPortOutLow(LPC_GPIO_LCD_DATA,LCD_DATA_PORT,((~data)&0x000F)<<LCD_D4_PIN);
	LCD_EN_PIN_HIGH();		//Set chan LCD_EN_PIN len 1
	DelayMs(1);
	LCD_EN_PIN_LOW();		//Keo chan LCD_EN_PIN xuong 0
	//LCD_Delay(10);
}	
void LCD_WriteCommand(uint32_t cmd){
	LCD_WaitBusy();
	LCD_SetCommandMode();
	LCD_Write4Bit(cmd>>4);
	LCD_Write4Bit(cmd);
}
void LCD_WriteData(uint8_t data){
	LCD_WaitBusy();
	LCD_EN_PIN_HIGH();
	LCD_SetDataMode();
	LCD_Write4Bit(data>>4);
	LCD_Write4Bit(data);
	LCD_SetCommandMode();
	LCD_EN_PIN_LOW();
}
uint8_t LCD_ReadStatus(){
	uint32_t status = 0;
	LCD_SetDataPortInput();
	LCD_SetCommandMode();
	LCD_SetReadMode();	
	DelayMs(1);// x :4bit
	LCD_EN_PIN_HIGH();
	status  = (Chip_GPIO_ReadValue(LPC_GPIO_LCD_DATA,LCD_DATA_PORT)&(0x000F<<LCD_D4_PIN));
	status = status<<4;
	LCD_EN_PIN_LOW();
	LCD_EN_PIN_HIGH();
	status |= (Chip_GPIO_ReadValue(LPC_GPIO_LCD_DATA,LCD_DATA_PORT)&(0x000F<<LCD_D4_PIN));
	LCD_EN_PIN_LOW();
	LCD_SetDataPortOutput();
	return (status);
}

uint8_t LCD_WaitBusy(){
	uint8_t status=0,wait_counter=0;
	do  {
		status = LCD_ReadStatus();
		wait_counter++;
	} 
	while (status & 0x80 && !(wait_counter==10));             /* Wait for busy flag                 */
	return (status);
}
void LCD_Clear (void) {
	LCD_WriteCommand(0x01);                  /* Display clear                      */
	LCD_WriteCommand(0x02);                  /* Display home                      */
}

void LCD_Delay(uint32_t cnt){
	uint32_t temp=0;
	temp = cnt*1;
 	while (temp--);
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LC
 * @param[in] 	Column and row value
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 * @return 		None
 ***********************************************************************/
void LCD_GotoXY(uint8_t col,uint8_t row)
{
	uint8_t address;
	col=col%16;
	if (row == 1){
		address = 0x80 |((row * 0x40) + col);
	}
	else {
		address = 0x80 | col;
	}
	LCD_WriteCommand(address);               /* Set DDRAM address counter to 0     */
}

void LCDScrollLeft(){
	LCD_WriteCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDScrollRight(){
	LCD_WriteCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/*********************************************************************//**
 * @brief 		Write a byte to LCD
 * @param[in] 	Value of byte want to write, value is a unsigned byte.
 * @return 		None
 ***********************************************************************/
void LcdPutChar(uint8_t c)
{
	LCD_WriteData(c);
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a three digit number
 * @param[in] 	Column , row and number value, should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number is a signed integer number
 * @return 		None
 ***********************************************************************/
void LCD_PutDigi3(uint8_t X, uint8_t Y, int number){
	LCD_GotoXY(X, Y);
	if (number<0){
		LcdPutChar('-'); // dau -
		number = -number;
	} else {
		LcdPutChar('+');
	}
	LcdPutChar('0' +(number%1000)/100);
	LcdPutChar('0' + (number%100)/10);
	LcdPutChar('0' + (number%10));		
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a four digit number
 * @param[in] 	Column , row and number value, should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number is a signed integer number
 * @return 		None
 ***********************************************************************/
void LcdPutDigi4(uint8_t X, uint8_t Y, int number){	
	LCD_GotoXY(X, Y);
	if (number<0){
		LcdPutChar('-'); // dau -
		number = -number;
	} else {
		LcdPutChar('+');
	}
	LcdPutChar('0' + (number%10000)/1000);
	LcdPutChar('0' + (number%1000)/100);
	LcdPutChar('0' + (number%100)/10);
	LcdPutChar('0' + (number%10));		
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a five digit number
 * @param[in] 	Column , row and number value, should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number is a signed integer number 
 * @return 		None
 ***********************************************************************/
void LcdPutDigi5(uint8_t X, uint8_t Y,int number){	
	LCD_GotoXY(X, Y);
	if (number<0){
		LcdPutChar('-'); // dau -
		number = -number;
	} else {
		LcdPutChar('+');
	}
	LcdPutChar('0' + (number%100000)/10000);
	LcdPutChar('0' + (number%10000)/1000);
	LcdPutChar('0' + (number%1000)/100);
	LcdPutChar('0' + (number%100)/10);
	LcdPutChar('0' + (number%10));		
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a sixten digit binary number
 * @param[in] 	Column , row and number value, should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number is an unsigned integer number
 * @return 		None
 ***********************************************************************/
void LcdPutBinary(uint8_t X, uint8_t Y,uint16_t number){
	LCD_GotoXY(X, Y);
	uint16_t tmp = 0x8000;
	while(tmp != 0){
		if((number & tmp) == 0){
			LcdPutChar('0');
		} else {
			LcdPutChar('1');
		}
		tmp = tmp>>1;
	}
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a byte
 * @param[in] 	Column , row and number value, should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number is an uint8_t number
 * @return 		None
 ***********************************************************************/
void LcdPrintChar (uint8_t column, uint8_t row, uint8_t val)
{
	uint8_t x1,y1;
	x1 = column % 16;
	y1 = row % 2;	
	DelayMs(1);
	LCD_GotoXY(x1, y1);
	DelayMs(1);
	LcdPutChar(val);
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a string
 * @param[in] 	Column , row and number value
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- String of character
 * @return 		None
 ***********************************************************************/
void LcdPrintString (uint8_t x, uint8_t y, char * ptr)
{
	uint8_t row, column;
	column = x % 16;
	row = y % 2;	
	DelayMs(1);
	LCD_GotoXY(column,row);
	DelayMs(1);
	while( *ptr != 0 ) 
	{
		LcdPutChar( *ptr );
		DelayMs(1);
		ptr ++;
	}
}

/*********************************************************************//**
 * @brief 		Goto coordinate cursor on Digit LCD, and write a limit quantity digits number 
 * @param[in] 	Column , row and number , quantity value,should be:
 * 				- Column : 0 -> 15
 * 				- Row	: 0 -> 1
 *				- number : is an integer number
 *				- quantity : quantity digit of number
 * @return 		None
 ***********************************************************************/
void LcdPrintNumber (uint8_t x, uint8_t y, int number, uint8_t quantity)
{
	int temp ;
	uint8_t index = 0;
	uint8_t x1 ;
	uint8_t tempArray [12] = {0,0,0,0,0,0,0,0,0,0,0,0}; 	
	if(number < 0){
		LcdPutChar(0x2D); // '-'
		number = -number;
		x1 = x+1;
	}
	else x1 = x;
	temp = number;
	while (temp != 0)
	{
		index ++;
		tempArray[index] = temp % 10;
		temp = temp / 10;
	}
	index = quantity % 16;
	while (index>=1)
	{
		LcdPrintChar(x1, y, tempArray[index] +'0');
		index --;
		x1 = x1 + 1;
	};
}

