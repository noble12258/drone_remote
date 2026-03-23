#ifndef _TASK_MANAGE_H_
#define _TASK_MANAGE_H_

#include "stm32f10x.h"

#define OLED_DISPLAY_PRIORITY		5

#define MASTER_TASK_PRIORITY		5
#define SECOND_TASK_PRIORITY		6

#define PAIR_PRIORITY				4

#define CPU_USAGE_PRIORITY		30

void MasterTask(void);
void SecondTask(void);
#endif
