#ifndef _BEEP_H_
#define _BEEP_H_

#include "stm32f10x.h"

#define BEEP_RCC_PORT		RCC_APB2Periph_GPIOA
#define BEEP_PIN				GPIO_Pin_8
#define BEEP_PORT				GPIOA

void BeepInit(void);
void BeepOn(void);
void BeepOff(void);
#endif
