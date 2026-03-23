#include "pair_freq.h"
#include "si24r1.h"
#include "spi.h"
#include "led.h"
#include "my_lib.h"
#include "log_lib.h"
#include "systick.h"
#include <rtthread.h>
#include <stdio.h>
#include <string.h>
#include "receive_packet.h"
#include "task_manage.h"
#include "crc_lib.h"

#define PAIR_PACKET_WIDTH 9		//对频数据包宽度
#define SEND_DETECT_DATA	0xB1
#define RECEIVE_DETECT_DATA	0xB2

static rt_thread_t pair_thread = RT_NULL;

static S_Pair pair = {STEP1};		//对频结构体变量

void Pair(void);
uint32_t GetId(void);
uint8_t SendData(uint8_t *sendData, uint16_t dataLen);

void pair_thread_entry(void *param)
{
	while(1){
		if(pair.status == PAIR_DONE){
			return; 	//对频成功，结束线程
		}
		Pair();
		rt_thread_mdelay(50);
	}
}

void Pair_Main(void)
{
	SPI1_Init();
	NRF24L01_Gpio_Init();
	if(NRF24L01_check() == 1){
		LogError("si24r11 init fail");
	}
	RF24L01_Init();
	RF24L01_Set_Mode(MODE_TX);
	
	//创建线程
	pair_thread = rt_thread_create("pair_thread", pair_thread_entry, RT_NULL, 512, PAIR_PRIORITY, 5);
	
	//启动线程
	if (pair_thread != RT_NULL){
		rt_thread_startup(pair_thread);
	} else {
		LogError("pair_thread start fail");
	}
}

//读取芯片ID
uint32_t GetId(void)
{
	uint32_t chipId[3] = {0};
	uint32_t mcuId = 0;
	
	chipId[0] = *(__IO u32 *)(0X1FFFF7F0);
	chipId[1] = *(__IO u32 *)(0X1FFFF7EC);
	chipId[2] = *(__IO u32 *)(0X1FFFF7E8);
	
	mcuId = (chipId[0] | chipId[1] | chipId[2]);
	return mcuId;
}

void Pair(void)
{
	static PairPacket sendPairPacket = {STEP1,{0x1F,0x2E,0x3D,0x4C,0x5B},5};
	uint32_t chipId = 0;
	uint8_t sendBuff[15] = {0};
	uint8_t receiveBuff[15] = {0};
	uint8_t reveiveData[20] = {0};
	int ret = -1;
	static uint8_t pairCount = 0;
	uint8_t receiveBufLength = 0;

	if(pair.status == PAIR_NOT){
		sendBuff[0] = SEND_DETECT_DATA;
		ret = SendData((uint8_t *)&sendBuff, 1);
		if(ret == 0){
			pair.detectStatus = DETECT_NORMAL;
		} else {
			pair.detectStatus = DETECT_NOT;
		}
	}
	else if(pair.status == PAIR_START){
		if(sendPairPacket.step == STEP1){
			LogInfo("pair step1");
			sendBuff[0] = sendPairPacket.step;
			SendData((uint8_t *)&sendBuff, 1);
		}
		else if(sendPairPacket.step == STEP3){
			LogInfo("pair step3");
			
			//使用芯片id数据作为数传地址和通道，使其通信地址唯一
			chipId = GetId();
			
			//新地址装入对频待发送数组
			sendPairPacket.addr[0] = *((uint8_t *)&chipId);		 
			sendPairPacket.addr[1] = *(((uint8_t *)&chipId)+1);	     
			sendPairPacket.addr[2] = *(((uint8_t *)&chipId)+2);		
			sendPairPacket.addr[3] = *(((uint8_t *)&chipId)+3);		
			sendPairPacket.addr[4] = sendPairPacket.addr[0];			
			
		 //频点装入数组
			sendPairPacket.freq_channel = 30;
			
			memcpy(sendBuff, (void *)&sendPairPacket, sizeof(sendPairPacket));
			
			for(int i = 0; i < 3; i++){		//为保证飞机接收到，发三次
				SendData((uint8_t *)&sendBuff, sizeof(sendPairPacket));//发送新的地址和频点给飞机
				rt_thread_mdelay(500);
			}
			
			//写入新的地址和通道
			NRF24L01_Set_TxAddr((uint8_t*)sendPairPacket.addr, TX_ADR_WIDTH);    //写TX节点地址 
			NRF24L01_Set_RxAddr(0, (uint8_t*)sendPairPacket.addr, RX_ADR_WIDTH); //设置RX节点地址,主要为了使能ACK	 
			NRF24L01_Write_Reg(RF_CH, sendPairPacket.freq_channel);		//设置RF通道（频点）
			
			sendPairPacket.step = STEP4;
		}
		else if(sendPairPacket.step == STEP4){
			LogInfo("pair step4");
			memcpy(sendBuff, (void *)&sendPairPacket, sizeof(sendPairPacket));
			SendData((uint8_t *)&sendBuff, sizeof(sendPairPacket));		//修改地址后再次发送，知道接收到飞机应答
		}
		
		pairCount++;
		if(pairCount > 60){
			LogInfo("pair fail");
			
			pair.status = PAIR_FAIL;
			pairCount = 0;
			
			//对频失败，地址和通道复位
			sendPairPacket.step = STEP1;
			sendPairPacket.addr[0] = 0x1F;
			sendPairPacket.addr[1] = 0x2E;
			sendPairPacket.addr[2] = 0x3D;
			sendPairPacket.addr[3] = 0x4C;
			sendPairPacket.addr[4] = 0x5B;
			sendPairPacket.freq_channel = 5;

			NRF24L01_Set_TxAddr((uint8_t*)sendPairPacket.addr, TX_ADR_WIDTH);    //写TX节点地址 
			NRF24L01_Set_RxAddr(0, (uint8_t*)sendPairPacket.addr, RX_ADR_WIDTH); //设置RX节点地址,主要为了使能ACK	 
			NRF24L01_Write_Reg(RF_CH, sendPairPacket.freq_channel);		//设置RF通道（频点）
			
		}

	}
	
	if(pair.status != PAIR_DONE){
		receiveBufLength = NRF24L01_Read_Rx_Payload((uint8_t *)&receiveBuff);
		if(receiveBufLength > 0){
			for(int i = 0; i < 20; i++){
				ret = AnalysisData(receiveBuff[i], reveiveData);
				if(ret == 0){
					if(pair.status == PAIR_NOT){		//由于si24r1的机制，该判断未使用，但保留为其他通信模块拓展
						if(reveiveData[0] == RECEIVE_DETECT_DATA){
							pair.detectStatus = DETECT_NORMAL;
						} 
					}
					else if(pair.status == PAIR_START){
						if(reveiveData[0] == STEP2){
							sendPairPacket.step = STEP3;
						}
						else if(reveiveData[0] == STEP5){
							pair.status = PAIR_DONE;
							LogInfo("pair done");
						}
					}
					break;
				}
			}
		} else {
			pair.detectStatus = DETECT_NOT;
		}
	}


}

uint8_t SendData(uint8_t *sendData, uint16_t dataLen)
{
	uint8_t sendArr[32] = {0};
	uint8_t arrLen = 0;
	uint8_t ret = 0;

	if(dataLen > 32){		//检查数据包长度
		LogError("lenth so much");
		return 1;
	}
	
	sendArr[arrLen++] = 0xAA;
	sendArr[arrLen++] = dataLen >> 8;
	sendArr[arrLen++] = dataLen;
	
	for(int i = 0; i < dataLen; i++){
		sendArr[arrLen++] = sendData[i];
	}
	
	sendArr[arrLen++] = crc8_maxim(sendData, dataLen);
	sendArr[arrLen++] = 0xAD;

	ret = NRF24L01_TxPacket(sendArr, arrLen);

	if(ret == TX_OK){
		return 0;
	} else {
		return 1;
	}
}

void GetPairInfo(S_Pair *exPair)
{
	StructCopy((uint8_t *)&pair, (uint8_t *)exPair, sizeof(pair));
}

void SetPairInfo(E_PairInfoSet pairInfo, PairStatus value)
{
	switch(pairInfo){
		case SET_PAIR_STATUS:
			pair.status = value;
			break;
		case SET_PAIR_STEP:
			pair.status = value;
			break;
		default:
			LogError("Option does not exist, set fail");
			break;
	}
}
