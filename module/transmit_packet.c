#include "transmit_packet.h"
#include "timer.h"
#include "usart.h"
#include "si24r1.h"
#include "key_handle.h"
#include "led.h"
#include "pair_freq.h"
#include "oled.h"
#include "receive_packet.h"
#include "my_lib.h"
#include "math_lib.h"
#include "log_lib.h"
#include "remote_calibrate.h"
#include <rtthread.h>
#include <stdio.h>
#include <string.h>
#include "adc.h"
#include "compile.h"
#include "queue_lib.h"
#include "task_manage.h"
#include "crc_lib.h"

#define TRANSMIT_BUFF_LENGHT	256		//发送数据缓冲区长度

uint8_t transmitBuff[TRANSMIT_BUFF_LENGHT] = {0};		//发送数据缓冲区
static S_Queue transmitQueue = {0};		//发送消息队列

static rt_mutex_t transmit_mutex = RT_NULL;

static E_SignalStatus signalStatus = SIGNAL_LOST;		//信号状态

//static uint8_t threadDelay = 5;

void TransmitHandle(void);

void TransmitInit(void)
{
	QueueInit(&transmitQueue, transmitBuff, TRANSMIT_BUFF_LENGHT);
	
	/* 创建互斥锁 */
	transmit_mutex = rt_mutex_create("transmit_mutex", RT_IPC_FLAG_PRIO);
	if (transmit_mutex == RT_NULL)
	{
		rt_kprintf("create dynamic transmit_mutex failed.\n");
		return;
	}

}

void TransmitHandle(void)
{
	uint8_t sendBuff[32] = {0};
	uint8_t data = 0;
	uint8_t ret = 0;
	static uint16_t signalLostCount = 0;
	S_Pair pair = {STEP1};
	uint8_t sendDataLen = 0;

	GetPairInfo(&pair);
	if(pair.status != PAIR_DONE){
		return;
	}
	
	rt_mutex_take(transmit_mutex, RT_WAITING_FOREVER);
	for(int i = 0; i < 32; i++){
		ret = QueueLength(&transmitQueue);
		if(ret > 0){
//			LogInfo("QueueLength: %d", ret);
			DeQueue(&transmitQueue, &data);		//从发送队列里取出数据
//			LogInfo("data: %x", data);
			sendBuff[sendDataLen++] = data;		//转移到发送数组中
		} else {
			break;
		}
	}
	rt_mutex_release(transmit_mutex);
	
	if(sendDataLen != 0){
//		LogInfo("sendDataLen: %d", sendDataLen);
		ret = NRF24L01_TxPacket(sendBuff, sendDataLen);		//发送数据
	}
	else {
		uint8_t heartPacket[5] = {0xA1, 0xB1, 0xC2, 0xD1, 0xE1};
		ret = NRF24L01_TxPacket(heartPacket, 5);		//发送心跳包
	}	

	//判断发送是否成功
//	LogInfo("ret: %d", ret);
	if(ret == TX_OK){
		signalLostCount = 0;
		signalStatus = SIGNAL_NORMAL;
//		threadDelay = 5;
	} else {

		if(signalLostCount < 150){
			signalLostCount++;
		} else {
//			LogInfo("signal lost");
			signalStatus = SIGNAL_LOST;
//			threadDelay = 10;		//信号丢失后，会以最大次数重发，会影响屏幕刷新，因此降低线程间隔时间
		}
	}
		
}

uint8_t StoreToTransmitQueue(uint8_t *sendData, uint16_t dataLen)
{
	uint8_t ret = 0;
	uint8_t sendArr[32] = {0};
	uint8_t arrLen = 0;
	
	if(dataLen > 27){		//判断包是否过长
		return 0;
	}
	
	//封包
	sendArr[arrLen++] = 0xAA;
	sendArr[arrLen++] = dataLen >> 8;
	sendArr[arrLen++] = dataLen;
	
	for(int i = 0; i < dataLen; i++){
		sendArr[arrLen++] = sendData[i];
	}
	
	sendArr[arrLen++] = crc8_maxim(sendData, dataLen);
	sendArr[arrLen++] = 0xAD;
	
	//存入发送队列
	rt_mutex_take(transmit_mutex, RT_WAITING_FOREVER);
	
	for(int i = 0; i < arrLen; i++){
//		LogInfo("sendArr[i]: %x", sendArr[i]);
		ret = EnQueue(&transmitQueue, sendArr[i]);
		if(ret == 1){
			LogError("transmit queue is full");
			return 0;
		}
	}
	
	rt_mutex_release(transmit_mutex);
	
	return 1;
}

E_SignalStatus GetSignalStatus(void)
{
	return signalStatus;
}
