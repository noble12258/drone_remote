#include "power_control.h"
#include "gpio.h"
#include "key.h"
#include "log_lib.h"

void PowerOn(void);
void PowerOff(void);

void PowerControlHandle(void)
{
	static uint8_t powerStatus = 0;
	static S_KeyScan powerKeyScan = {0, 0, VCC_DETECT_IN_PORT, VCC_DETECT_IN_PIN};
	static uint8_t powerOffCount = 0;
	static uint8_t powerOffTimeCount = 0;
	uint8_t powerKey = 0;
	
	powerKey = KeyScan(&powerKeyScan, 0);
	if(powerKey == 1){
		if(powerStatus == 0){
			LogDma("power on");
			PowerOn();
			powerStatus = 1;
		}
		else if(powerStatus == 1){
			powerOffCount++;
			if(powerOffCount == 2){
				LogDma("power off");
				PowerOff();
				powerStatus = 0;
				powerOffCount = 0;
				powerOffTimeCount = 0;
			}
		}
	}
	
	if(powerOffCount == 1){
		powerOffTimeCount++;
		if(powerOffTimeCount == 10 && powerOffCount != 2){
			powerOffCount = 0;
			powerOffTimeCount = 0;
		}
	}

}

void PowerOn(void)
{
//	VCC_DETECT_OUT_HIGH;
	GPIO_SetBits(VCC_DETECT_OUT_PORT, VCC_DETECT_OUT_PIN);
}

void PowerOff(void)
{
//	VCC_DETECT_OUT_LOW;
	GPIO_ResetBits(VCC_DETECT_OUT_PORT, VCC_DETECT_OUT_PIN);
}
