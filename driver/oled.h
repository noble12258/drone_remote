#ifndef _oled_h_
#define _oled_h_

#include "stm32f10x.h"
#include "OledFont.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据



void OLED_Init(void); 
void OLED_WR_Byte(unsigned Data,unsigned DataType);
void Write_OLED_Command(unsigned char IIC_Command);
void Write_OLED_Data(unsigned char IIC_Data);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_On(void);
void OledDisplayString(u8 row,u8 column,u8 str[]);
void OLED_ShowNum(u8 row,u8 column,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 row,u8 column,char *chr,u8 size);
void OLED_ShowChar(u8 row,u8 column,u8 chr,u8 Char_Size);
void OledClearSpecifyRow(uint8_t row, uint8_t column);
void OledDisplayChinese(uint8_t row, uint8_t column, const u8 strChinese[][16],u8 num);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, const unsigned char Bmp[]);
void Ssd1306ClearRegion(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void OledClearBlock(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
#endif


