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
	
static S_KeyValue keyValue = {0};		//��ֵ
static S_KeyValue tempKeyValue = {0};		//��ֵ�Ŀ�����������Ļ����

static S_SwitchValue switchValue = {0};		//ҡ�ۿ���

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
	memset((void *)&keyValue, 0, sizeof(keyValue));

	/* OLED四键映射：左上KEY1=上，左下KEY2=下，右上KEY4=确定，右下KEY3=返回 */
	keyValue.up = KeyScan(&switch1, 0);
	keyValue.down = KeyScan(&switch2, 0);
	keyValue.cancel = KeyScan(&switch3, 0);
	keyValue.ok = KeyScan(&switch4, 0);

	if(keyValue.up == 1 || keyValue.down == 1 || keyValue.ok == 1 || keyValue.cancel == 1){
		memcpy((void *)&tempKeyValue, (void *)&keyValue, sizeof(keyValue));
	}

	/* 注意：原理图中PA3已分配给右摇杆，无ADC按键硬件，ADC按键功能已禁用 */
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
