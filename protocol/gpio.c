#include "gpio.h"

void PowerControlGpioInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(VCC_DETECT_IN_RCC_PORT | VCC_DETECT_OUT_RCC_PORT,ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Mode_IPU;
	GPIO_Init(VCC_DETECT_IN_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = VCC_DETECT_OUT_PIN;
	GPIO_Init(VCC_DETECT_OUT_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(VCC_DETECT_OUT_PORT, VCC_DETECT_OUT_PIN);
}

