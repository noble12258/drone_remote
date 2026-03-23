#include "oled.h"
#include "systick.h"
#include "i2c.h"
#include "led.h"

//屏幕类型选择
//#define SSD1306_OLED		1
#define SH1106_OLED		1

/*******************************************************************************
* 函 数 名         : OLED_Init()
* 函数功能		     : OLED初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void OLED_Init(void)
{
#ifdef SH1106_OLED
	delay_ms(200);
	OLED_WR_Byte(0xAE,OLED_CMD);//set display display ON/OFF,AFH/AEH
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0x10,OLED_CMD);
	OLED_WR_Byte(0x40,OLED_CMD);//set display start line:COM0
	OLED_WR_Byte(0xB0,OLED_CMD);
	OLED_WR_Byte(0x81,OLED_CMD);//set contrast control
	OLED_WR_Byte(0xCF,OLED_CMD);

	OLED_WR_Byte(0xA1,OLED_CMD);//entire display on: A4H:OFF/A5H:ON
	OLED_WR_Byte(0xC8,OLED_CMD); //该指令控制显示方向显示方向0xc8或者0xc0

	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_WR_Byte(0xA7,OLED_CMD);//set normal/inverse display: A6H:normal/A7H:inverse

	OLED_WR_Byte(0xA8,OLED_CMD);//set multiplex ratio
	OLED_WR_Byte(0x3F,OLED_CMD);//1/64duty
	OLED_WR_Byte(0xD3,OLED_CMD);//set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	OLED_WR_Byte(0xD5,OLED_CMD);//set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//105Hz
	OLED_WR_Byte(0xD9,OLED_CMD);//Dis-charge /Pre-charge Period Mode Set
	OLED_WR_Byte(0xF1,OLED_CMD);//
	OLED_WR_Byte(0xDA,OLED_CMD);//Common Pads Hardware Configuration Mode Set
	OLED_WR_Byte(0x12,OLED_CMD);//
	OLED_WR_Byte(0xDB,OLED_CMD);//set vcomh deselect level
	OLED_WR_Byte(0x40,OLED_CMD);//VCOM = β X VREF = (0.430 + A[7:0] X 0.006415,OLED_CMD) X VREF
	OLED_WR_Byte(0xA4,OLED_CMD);
	OLED_WR_Byte(0xA6,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);//set display display ON/OFF,AEH/AFH
#elif	SSD1306_OLED
	delay_ms(200);
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address0
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address16
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address176
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0x08,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
#endif

}



/*******************************************************************************
* 函 数 名         : Write_OLED_Command()
* 函数功能		     : OLED写命令
* 输    入         : OLED_Command(命令)
* 输    出         : 无
*******************************************************************************/
void Write_OLED_Command(unsigned char OLED_Command)
{
  IIC_Start();
  IIC_Send_Byte(0x78);       //Slave address,SA0=0
	IIC_Slave_Ack();	
  IIC_Send_Byte(0x00);			//D/C#=0;写命令
	IIC_Slave_Ack();	
  IIC_Send_Byte(OLED_Command); 
	IIC_Slave_Ack();	
  IIC_Stop();
}
//
/*******************************************************************************
* 函 数 名         : Write_OLED_Data()
* 函数功能		     : OLED写数据
* 输    入         : OLED_Data(数据)
* 输    出         : 无
*******************************************************************************/
void Write_OLED_Data(unsigned char OLED_Data)
{
  IIC_Start();
  IIC_Send_Byte(0x78);			// R/W#=0;读写地址及模式选择，这里SA=0，R/W#=0（写模式）
	IIC_Slave_Ack();	
  IIC_Send_Byte(0x40);			//D/C#=1;写数据
	IIC_Slave_Ack();	
  IIC_Send_Byte(OLED_Data);//写入数据
	IIC_Slave_Ack();	
  IIC_Stop();
}




/*******************************************************************************
* 函 数 名         : OLED_WR_Byte()
* 函数功能		     : OLED命令/数据模式选择
* 输    入         : Data(数据)
										 DataType(数据类型：数据/命令)
* 输    出         : 无
*******************************************************************************/
void OLED_WR_Byte(unsigned Data,unsigned DataType)
{
	if(DataType)
	{
   Write_OLED_Data(Data);//写数据，cmd=0

	}
	else 
	{
   Write_OLED_Command(Data);	//写命令，cmd=1
	}
}



/*******************************************************************************
* 函 数 名         : OLED_Display_On()
* 函数功能		     : 开启OLED显示 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


/*******************************************************************************
* 函 数 名         : OLED_Display_Off()
* 函数功能		     : 关闭OLED显示
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   	


/*******************************************************************************
* 函 数 名         : OLED_Clear()
* 函数功能		     : 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
#ifdef SH1106_OLED
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
#elif	SSD1306_OLED
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
#endif
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

//清除指定行
void OledClearSpecifyRow(uint8_t row, uint8_t column)
{
	uint8_t i,n;
	
	for(i = row; i <= row + 1; i++)  
	{  
		OLED_WR_Byte (0xb0 + i,OLED_CMD);    //设置页地址（0~7）
#ifdef SH1106_OLED
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
#elif	SSD1306_OLED
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
#endif
		OLED_WR_Byte (0x10 + column, OLED_CMD);      //设置显示位置—列高地址   
		
		for(n = 0; n < 128; n++){		//更新显示
			OLED_WR_Byte(0,OLED_DATA);
		}
		
	} 
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
#ifdef SH1106_OLED
		OLED_WR_Byte((x&0x0f) + 0x02,OLED_CMD);		//设置显示位置—列低地址
#elif	SSD1306_OLED
		OLED_WR_Byte((x&0x0f),OLED_CMD);		//设置显示位置—列低地址
#endif
	
}   

/*******************************************************************************
* 函 数 名         : OLED_On()
* 函数功能		     : oled满屏显示
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++){  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
#ifdef SH1106_OLED
		OLED_WR_Byte (0x02,OLED_CMD);      //设置显示位置—列低地址
#elif	SSD1306_OLED
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
#endif
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		
		for(n=0;n<128;n++){		//更新显示
			OLED_WR_Byte(0xff,OLED_DATA);
		}			
	} 
}

//显示bmp图片，适配SH1106屏，使用PCtoLCD2022完美版工具取模，
//配置参数为：阴码，列行式，点阵16，索引8，逆向，十六进制，C51格式
//参数：x0，起始横向坐标，范围0~15
//			y0，起始纵向坐标，范围0~8
//			x1，起始横向坐标，范围0~15
//			y1，起始纵向坐标，范围0~8
//			Bmp[]，图像数据
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1, const unsigned char Bmp[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
	x0 = x0 * 8;		//偏移8倍个像素点，可调整
	x1 = x1 *  8;
	
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(Bmp[j++],OLED_DATA);	    	
	    }
	}
} 

/**
* @brief 显示中文(多行显示)，适配SH1106屏，使用PCtoLCD2022完美版工具取模
				配置参数为：阴码，列行式，点阵16，索引8，逆向，十六进制，C51格式
* @param row：0~7
* @param column：0~15
* @param strChinese：中文数组
* @param num：1~32字数
*/
void OledDisplayChinese(uint8_t row, uint8_t column, const u8 strChinese[][16], u8 num)
{
	uint8_t availableNum = 0;
	uint8_t diplayCount = 0;		//换行计数
	uint8_t enterCount = 0;		//换行次数
	const uint8_t offsetPixel = 8;		//列偏移8个像素，也就是半个汉字
	
	if(column == 1){
		availableNum = 7;
	} else {
		availableNum = 8 - (column / 2 + column % 2);		//计算一行能显示汉字的个数
	}
	
	OLED_Set_Pos(column * offsetPixel, row);		//设置坐标，列偏移4个像素点，行偏移8个像素点
	for(int arrIndex = 0; arrIndex < num; arrIndex++){		//显示一行汉字上半部分
		for(int i = 0; i < 16; i++){		//显示一个汉字上半部分
			OLED_WR_Byte(strChinese[arrIndex*2][i], OLED_DATA);		//发送显示数据
		}
		
		diplayCount++;
		if(diplayCount == (availableNum)){		//一行不够更新一次坐标
			diplayCount = 0;
			enterCount++;
			OLED_Set_Pos(column * offsetPixel, row + enterCount * 2);		//一行不够进行换行
		}

	}
	
	//清零计数值，用于下半部分显示
	diplayCount = 0;
	enterCount = 0;
	
	OLED_Set_Pos(column * offsetPixel, row + 1);		//往下偏移一行
	
	for(int arrIndex = 0; arrIndex < num; arrIndex++){		//显示一行汉字下半部分
		for(int i = 0; i < 16; i++){		//显示一个汉字下半部分
			OLED_WR_Byte(strChinese[arrIndex*2+1][i], OLED_DATA);
		}
		
		diplayCount++;
		if(diplayCount == (availableNum)){
			diplayCount = 0;
			enterCount++;
			OLED_Set_Pos(column * offsetPixel, row + enterCount * 2 + 1);
		}

	}
}

/**
* @brief 在指定位置显示一个字符,包括部分字符
* @param x:0~15
* @param y:0~6
* @param size:选择字体 16/12
*/
void OLED_ShowChar(u8 row,u8 column,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值		
		column = column*8;
		if(column>Max_Column-1){column=0;row=row+2;}
		if(Char_Size ==16){
			OLED_Set_Pos(column,row);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(column,row+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
		} else {	
				OLED_Set_Pos(column,row);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
		}
			
}

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  

//显示数字
//row：行数（0~6）
//column：列数（0~15）
//len :数字的位数(1~16)
//num:数值(0~4294967295) 
//size:字体大小(8或16)
void OLED_ShowNum(u8 row,u8 column,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;		
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
//			OLED_ShowChar(row+(size/2)*t,column,' ',size);
				OLED_ShowChar(row,column+t,' ',size);
				continue;
			}else enshow=1; 
		 	 
		}
//	 	OLED_ShowChar(row+(size/2)*t,column,temp+'0',size); 
			OLED_ShowChar(row,column+t,temp+'0',size);
	}
} 

//显示字符串
//row：行数（0~6）
//column：列数（0~15）
//*chr :字符串指针 
//size:字体大小(8或16)
void OLED_ShowString(u8 row,u8 column,char *chr,u8 size)
{
	for(int i = 0; chr[i] != '\0'; i++){
		OLED_ShowChar(row, column, chr[i], size);
		column += 1;
	}
}

void Ssd1306ClearRegion(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	for(int i = 0; i < (y2 - y1); i++){
		OLED_Set_Pos(x1, y1 + i);
		
		for(int j = 0; j < (x2 - x1); j++) {
			Write_OLED_Data(0);
		}
	}
	
}

void OledClearBlock(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	x1 *= 8;
	x2 *= 8;
	
	Ssd1306ClearRegion(x1, y1, x2, y2);
}

