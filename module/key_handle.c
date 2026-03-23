#include "key_handle.h"
#include "systick.h"
#include "led.h"
#include "my_lib.h"
#include "adc.h"
#include "log_lib.h"
#include <rtthread.h>
#include <string.h>
#include "transmit_packet.h"
#include "key.h"
#include "filter_lib.h"
#include "task_manage.h"
#include "oled_display.h"
	
static S_KeyValue keyValue = {0};		//键值
static S_KeyValue tempKeyValue = {0};		//键值的拷贝，用于屏幕操作

static S_SwitchValue switchValue = {0};		//摇臂开关

static S_KeyScan switch1 = {0, 0, SWITCH_1_PORT, SWITCH_1_PIN};
static S_KeyScan switch2 = {0, 1, SWITCH_2_PORT, SWITCH_2_PIN};
static S_KeyScan switch3 = {0, 1, SWITCH_3_PORT, SWITCH_3_PIN};
static S_KeyScan switch4 = {0, 0, SWITCH_4_PORT, SWITCH_4_PIN};
static S_KeyScan switch5 = {0, 0, SWITCH_5_PORT, SWITCH_5_PIN};
static S_KeyScan switch6 = {0, 0, SWITCH_6_PORT, SWITCH_6_PIN};

uint8_t KeyScan(S_KeyScan *keyScan, bool scanMode);

void KeyAnalysis(void);
void SwitchHandle(void);

void KeyHandle(void)
{
	KeyAnalysis();
	SwitchHandle();
}

void KeyAnalysis(void)
{
	float keyAdcVoltage = 0;
	static uint8_t keyPressFlag = 0;
	S_KeyScan cancelKey = {0, 1, KEY_2_PORT, KEY_2_PIN};
	static float lastKey = 0;
	
	keyValue.cancel = KeyScan(&cancelKey, 0);
	if(keyValue.cancel == 1){
		LogDma("key cancel");
		memcpy((void *)&tempKeyValue, (void *)&keyValue, sizeof(keyValue));
	}
	const uint16_t *pAdcValue = NULL;
	pAdcValue = GetAdcValue();
	
	keyAdcVoltage = ((float)pAdcValue[3] * 3.3f / 4096.0f);	//计算按键的电压
//		LogDma("%f", keyAdcVoltage);

	/*消抖*/
	keyAdcVoltage = RangeFilterFloat(keyAdcVoltage, &lastKey, 0.1f);
	if(keyAdcVoltage <= 0.2){	//无按键按下时电压应小于2V
		keyAdcVoltage = 0;
	}

	if(keyPressFlag == 0){
		if(keyAdcVoltage > 0.47 && keyAdcVoltage <= 1.7){
			rt_thread_mdelay(10);
			keyAdcVoltage = ((float)pAdcValue[3] * 3.3f / 4096.0f);	//计算按键的电压
			keyAdcVoltage = RangeFilterFloat(keyAdcVoltage, &lastKey, 0.1f);
			
			if(keyAdcVoltage > 0.47 && keyAdcVoltage <= 1.7){

					if(keyAdcVoltage >= 1.5 && keyAdcVoltage <= 1.7){
						keyPressFlag = 1;
						LogDma("key up");
						keyValue.up = 1;
					}
					else if(keyAdcVoltage >= 1.0 && keyAdcVoltage <= 1.2){
						keyPressFlag = 1;
						LogDma("key left");
						keyValue.left = 1;
					}
					else if(keyAdcVoltage >= 0.75 && keyAdcVoltage <= 0.85){
						keyPressFlag = 1;
						LogDma("key down");
						keyValue.down = 1;
					}
					else if(keyAdcVoltage >= 0.58 && keyAdcVoltage <= 0.68){
						keyPressFlag = 1;
						LogDma("key right");
						keyValue.right = 1;
					}
					else if(keyAdcVoltage > 0.47 && keyAdcVoltage <= 0.57){
						keyPressFlag = 1;
						LogDma("key ok");
						keyValue.ok = 1;					
					}

			}

			
			if(keyPressFlag == 1){
				memcpy((void *)&tempKeyValue, (void *)&keyValue, sizeof(keyValue));
			}
			
		}
	}
	else {
		keyValue.up = 0;
		keyValue.left = 0;
		keyValue.down = 0;
		keyValue.right = 0;
		keyValue.ok = 0;			
	}
	
	if(keyAdcVoltage == 0){
		keyPressFlag = 0;
	}
	
}

void SwitchHandle(void)
{
	switchValue.switch1 = KeyScan(&switch1, 1);
	switchValue.switch2 = KeyScan(&switch2, 1);
	switchValue.switch3 = KeyScan(&switch3, 1);
	switchValue.switch4 = KeyScan(&switch4, 1);
	switchValue.switch5 = KeyScan(&switch5, 1);
	switchValue.switch6 = KeyScan(&switch6, 1);	
}

void GetSwitchValue(S_SwitchValue *exSwitchValue)
{
	StructCopy((uint8_t *)&switchValue, (uint8_t *)exSwitchValue, sizeof(switchValue));
}

void GetKeyValue(S_KeyValue *exKey)
{
	memcpy((void *)exKey, (void *)&tempKeyValue, sizeof(tempKeyValue));
	memset((void *)&tempKeyValue, 0, sizeof(tempKeyValue));
}
