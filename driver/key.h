#ifndef	_KEY_H_
#define	_KEY_H_

#include "stm32f10x.h"
#include <stdbool.h>

#define SWITCH_RCC_SUM	SWITCH_1_RCC | SWITCH_2_RCC | SWITCH_3_RCC | SWITCH_4_RCC | SWITCH_5_RCC | SWITCH_6_RCC

#define SWITCH_1_RCC		RCC_APB2Periph_GPIOC
#define SWITCH_1_PORT		GPIOC
#define SWITCH_1_PIN		GPIO_Pin_13

#define SWITCH_2_RCC		RCC_APB2Periph_GPIOB
#define SWITCH_2_PORT		GPIOB
#define SWITCH_2_PIN		GPIO_Pin_12

#define SWITCH_3_RCC		RCC_APB2Periph_GPIOB
#define SWITCH_3_PORT		GPIOB
#define SWITCH_3_PIN		GPIO_Pin_15

#define SWITCH_4_RCC		RCC_APB2Periph_GPIOA
#define SWITCH_4_PORT		GPIOA
#define SWITCH_4_PIN		GPIO_Pin_8

#define SWITCH_5_RCC		RCC_APB2Periph_GPIOB
#define SWITCH_5_PORT		GPIOB
#define SWITCH_5_PIN		GPIO_Pin_6

#define SWITCH_6_RCC		RCC_APB2Periph_GPIOB
#define SWITCH_6_PORT		GPIOB
#define SWITCH_6_PIN		GPIO_Pin_5

typedef struct{
	uint8_t keyPushFlag;		//用于记录按键按下标志位，初始化为0即可
	bool effectiveLevel;		//有效电平，0：低电平有效，1：高电平有效
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}S_KeyScan;		//由于要存储数据，所以需声明成static类型

void KeyInit(void);
uint8_t KeyScan(S_KeyScan *keyScan, bool scanMode);
#endif
