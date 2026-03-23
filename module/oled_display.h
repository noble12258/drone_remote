#ifndef _oled_display_h
#define _oled_display_h

#include "stm32f10x.h"

typedef struct
{
	uint8_t current;	//当前状态索引号
	uint8_t last;		//上
	uint8_t next;		//下
	uint8_t left;		//左
	uint8_t right;		//右
	uint8_t enter;		//确定
	uint8_t back;		//退出
	void (*currentP)(void);		//函数指针，指向要跳转的函数
}S_OledMenu;

void OledDisplay_Main(void);
#endif
