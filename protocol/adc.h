#ifndef _adc_h_
#define _adc_h_

#include "stm32f10x.h"

typedef struct{
	uint16_t leftX;
	uint16_t leftY;
	uint16_t rightX;
	uint16_t rightY;
	float batteryVoltage;
	float key1;
}AdcValue;

void AdcGpioInit(void);
void AdcInit(void);
void SetAdcInfo(const AdcValue *exAdcValue);
const uint16_t *GetAdcValue(void);
#endif


