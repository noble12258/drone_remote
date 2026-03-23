#include "task_manage.h"
#include "led_status.h"
#include "key_handle.h"
#include "transmit_packet.h"
#include "receive_packet.h"
#include "remote.h"
#include "remote_calibrate.h"
#include <rtthread.h>
#include "log_lib.h"
#include "remote_calibrate.h"

static rt_thread_t master_task_thread = RT_NULL;
static rt_thread_t second_task_thread = RT_NULL;

void master_task_thread_entry(void *param)
{
	while(1){
		TransmitHandle();
		
		ReceiveHandle();
		
		LedHandle();		
		
		rt_thread_mdelay(5);
	}
}

//飞控主任务
void MasterTask(void)
{	
	//创建线程
	master_task_thread = rt_thread_create("master_task_thread", master_task_thread_entry, RT_NULL, 2048, MASTER_TASK_PRIORITY, 5);
	
	//启动线程
	if (master_task_thread != RT_NULL){
		rt_thread_startup(master_task_thread);
	} else {
		LogError("master_task_thread start fail");
	}
}

void second_task_thread_entry(void *param)
{
	while(1){
		RemoteHandle();
		KeyHandle();
		RemoteCalibrateHandle();
		
		rt_thread_mdelay(10);
	}
}

//次要任务
void SecondTask(void)
{	
	//创建线程
	second_task_thread = rt_thread_create("second_task_thread", second_task_thread_entry, RT_NULL, 2048, SECOND_TASK_PRIORITY, 5);
	
	//启动线程
	if (second_task_thread != RT_NULL){
		rt_thread_startup(second_task_thread);
	} else {
		LogError("second_task_thread start fail");
	}
	
}
