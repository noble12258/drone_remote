#include "nvic.h"

void NvicConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;    

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             //优先级组别2

	//DMA1中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级，数值越小，优先级越高
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//响应优先级，数值越小，优先级越高。
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            
	NVIC_Init(&NVIC_InitStructure);                   

//	//外部中断10~15
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//EXTI中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;		//抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//响应优先级，数值越小，优先级越高。
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);		//根据指定的参数初始化 NVIC 寄存器
//	
//	//任务调度定时器中断
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;    
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		//抢占优先级，数值越小，优先级越高
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//响应优先级，数值越小，优先级越高。
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            
//	NVIC_Init(&NVIC_InitStructure);   	
}







