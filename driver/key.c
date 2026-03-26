#include "key.h"
#include "systick.h"

//按键引脚初始化
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(SWITCH_RCC_SUM, ENABLE);  

	GPIO_InitStructure.GPIO_Pin = SWITCH_1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_1_PORT, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = SWITCH_2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_2_PORT, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = SWITCH_3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_3_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = SWITCH_4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_4_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = SWITCH_5_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_5_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = SWITCH_6_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//下拉输入，无输入时是低电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SWITCH_6_PORT, &GPIO_InitStructure); 
}

/*按键扫描
scanMode: 1 连续扫描; 0 不连续扫描（按下后需松开才能再按下）
return: 0, 按键未按下; 1, 按键已按下
*/
uint8_t KeyScan(S_KeyScan *keyScan, bool scanMode)
{
	uint8_t keyValue = 2;
	
	if(scanMode == 1) {
		keyScan->keyPushFlag = 0;
	}

	if(keyScan->keyPushFlag == 0 && GPIO_ReadInputDataBit(keyScan->GPIOx, keyScan->GPIO_Pin) == keyScan->effectiveLevel){
		delay_ms(10);		//延时消抖
		keyScan->keyPushFlag = 1;
		
		if(GPIO_ReadInputDataBit(keyScan->GPIOx, keyScan->GPIO_Pin) == keyScan->effectiveLevel) {
			keyValue = 1;
		} 
		
	} else {
			keyValue = 0;
	}
	
	if(GPIO_ReadInputDataBit(keyScan->GPIOx, keyScan->GPIO_Pin) == !keyScan->effectiveLevel){		//若上一次按键按下没松开，则下一次不进入扫描
		keyScan->keyPushFlag = 0;
	}
	
	return keyValue;
}
