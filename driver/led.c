#include "led.h"

void LedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED_RGB_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
	GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
	GPIO_Init(LED_GREEN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
	GPIO_Init(LED_BLUE_PORT, &GPIO_InitStructure);

	/* 共阳极：高电平熄灭，默认全灭 */
	GPIO_SetBits(LED_RED_PORT, LED_RED_PIN);
	GPIO_SetBits(LED_GREEN_PORT, LED_GREEN_PIN);
	GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
}

static void RgbSet(uint8_t r, uint8_t g, uint8_t b)
{
	if(r) GPIO_ResetBits(LED_RED_PORT, LED_RED_PIN);
	else  GPIO_SetBits(LED_RED_PORT, LED_RED_PIN);

	if(g) GPIO_ResetBits(LED_GREEN_PORT, LED_GREEN_PIN);
	else  GPIO_SetBits(LED_GREEN_PORT, LED_GREEN_PIN);

	if(b) GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN);
	else  GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
}

void TopLedColorSet(const LedColor ledColor)
{
	static LedColor lastColor = BLACK;

	if(lastColor == ledColor){
		return;
	} else {
		lastColor = ledColor;
	}

	switch(ledColor){
		case RED:     RgbSet(1, 0, 0); break;
		case GREEN:   RgbSet(0, 1, 0); break;
		case BLUE:    RgbSet(0, 0, 1); break;
		case YELLOW:  RgbSet(1, 1, 0); break;
		case PURPLE:  RgbSet(1, 0, 1); break;
		case CYAN:    RgbSet(0, 1, 1); break;
		case WHITE:   RgbSet(1, 1, 1); break;
		default:      RgbSet(0, 0, 0); break;
	}
}

void TopLedBlink(LedColor ledColor, uint8_t frequency, uint16_t delayMs)
{
	static uint16_t blinkCount = 0;
	uint16_t period = 0;

	period = 1000 / (delayMs * frequency * 2);
	if(blinkCount == 0){
		TopLedColorSet(ledColor);
	}
	else if(blinkCount == period){
		TopLedColorSet(BLACK);
	}

	blinkCount++;
	if(blinkCount == (2 * period)){
		blinkCount = 0;
	}
}

void RgbLedBlinkFast(LedColor ledColor)
{
	static uint8_t blinkCount = 0;

	if(blinkCount == 0){
		TopLedColorSet(ledColor);
	}
	else if(blinkCount == 5){
		TopLedColorSet(BLACK);
	}

	blinkCount++;
	if(blinkCount == 9){
		blinkCount = 0;
	}
}

void RgbLedBlinkSlow(LedColor ledColor)
{
	static uint8_t blinkCount = 0;

	if(blinkCount == 0){
		TopLedColorSet(ledColor);
	}
	else if(blinkCount == 12){
		TopLedColorSet(BLACK);
	}

	blinkCount++;
	if(blinkCount == 24){
		blinkCount = 0;
	}
}
