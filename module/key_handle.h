#ifndef _KEY_HANDLE_H_
#define _KEY_HANDLE_H_

#include "stm32f10x.h"
#include "queue_lib.h"

typedef struct
{
	uint8_t up;		//上
	uint8_t down;	//下
	uint8_t left;	//左
	uint8_t right;	//右
	uint8_t ok;		//确认
	uint8_t cancel;		//取消
}S_KeyValue;

typedef struct{
	uint8_t switch1;
	uint8_t switch2;
	uint8_t switch3;
	uint8_t switch4;
	uint8_t switch5;
	uint8_t switch6;
}S_SwitchValue;

void GetSwitchValue(S_SwitchValue *exSwitchValue);
void KeyHandleInit(void);
void GetKeyValue(S_KeyValue *exKey);
void KeyHandle(void);
#endif

