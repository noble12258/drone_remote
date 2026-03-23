#ifndef _led_h_
#define _led_h_

#include "stm32f10x.h"

/* 共阳极RGB LED引脚定义（低电平点亮，高电平熄灭） */
#define LED_RGB_RCC        RCC_APB2Periph_GPIOB

#define LED_RED_PORT       GPIOB
#define LED_RED_PIN        GPIO_Pin_7

#define LED_GREEN_PORT     GPIOB
#define LED_GREEN_PIN      GPIO_Pin_8

#define LED_BLUE_PORT      GPIOB
#define LED_BLUE_PIN       GPIO_Pin_9

typedef enum{
	RED = 1,
	GREEN,
	BLUE,
	YELLOW,
	PURPLE,
	CYAN,
	WHITE,
	BLACK,
}LedColor;

void LedInit(void);
void RgbLedBlinkFast(LedColor ledColor);
void TopLedColorSet(const LedColor ledColor);
void RgbLedBlinkSlow(const LedColor ledColor);
void TopLedBlink(LedColor ledColor, uint8_t frequency, uint16_t delayMs);
#endif
