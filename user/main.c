#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "spi.h"
#include "si24r1.h"
#include "usart.h"
#include "timer.h"
#include "nvic.h"
#include "adc.h"
#include "key.h"
#include "pair_freq.h"
#include "transmit_packet.h"
#include "oled.h"
#include "i2c.h"
#include "oled_display.h"
#include "transmit_packet.h"
#include "remote_calibrate.h"
#include "log_lib.h"
#include <rtthread.h>
#include "beep.h"
#include "power_control.h"
#include "gpio.h"
#include "key_handle.h"
#include "led_status.h"
#include "voltage_detect.h"
#include "remote.h"
#include "receive_packet.h"
#include "cpu_usage.h"
#include "task_manage.h"

int main(void)
{
	PowerControlGpioInit();		//电源按键初始化
	
	//外设初始化
//	UartInit();		//打印串口已在 rt_hw_board_init()初始化
	I2cInit();
	SPI1_Init();
	
	LedInit();		//状态灯初始化
	KeyInit();		//按键初始化
	BeepInit();		//蜂鸣器初始化

	AdcInit();
	TimerInit();		//定时器触发ADC采样

	OledDisplay_Main();		//显示线程初始化

	Pair_Main();		//对频线程初始化
	TransmitInit();		//发送功能初始化
	ReceiveInit();		//接收功能初始化
	
	RemoteInit();		//遥控状态初始化
	RemoteCalibrateInit();		//摇杆校准初始化

//	NvicConfig();		//中断配置初始化
	MasterTask();		//主任务初始化
	SecondTask();		//次级任务初始化
	
	CpuUsage_Main();		//cpu利用率初始化
	
	rt_thread_mdelay(200);
	LogInfo("system init done.");
	
	while(1){
		PowerControlHandle();		//电源控制处理
		VoltageDetect();		//电压检测
		rt_thread_mdelay(100);
	}
}
