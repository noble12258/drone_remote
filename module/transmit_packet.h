#ifndef _sendpacket_h_
#define _sendpacket_h_

#include "stm32f10x.h"

typedef enum{
	CMD_ROCKER_DATA = 0xA3,
	CMD_ACC_CAL = 0xA4,		//加速计校准
	CMD_GYRO_CAL = 0xA5,		//陀螺仪校准
	CMD_COMPASS_CAL = 0xA6,		//磁力计校准
	CMD_IMU = 0xA7,		//请求陀螺仪数据
}E_Cmd;

typedef enum{
	SIGNAL_LOST = 0,
	SIGNAL_NORMAL,
}E_SignalStatus;

void analyze_packet(void);
void TransmitInit(void);
uint8_t SendData(uint8_t *sendData, uint16_t dataLen);
E_SignalStatus GetSignalStatus(void);
uint8_t StoreToTransmitQueue(uint8_t *sendData, uint16_t dataLen);
void TransmitHandle(void);
#endif

