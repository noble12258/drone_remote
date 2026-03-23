#include "voltage_detect.h"
#include "adc.h"
#include <stddef.h>
#include "filter_lib.h"

static float powerVoltage = 0;

void VoltageDetect(void)
{
	const uint16_t *pAdcValue = NULL;
	static float lastVoltage = 0;
	
	pAdcValue = GetAdcValue();
	powerVoltage = 2.0f*(pAdcValue[4]*3.3f/4096.0f);		//计算电压
	
	powerVoltage = RangeFilterFloat(powerVoltage, &lastVoltage, 0.03f);
}

float GetPowerVoltage(void)
{
	return powerVoltage;
}

