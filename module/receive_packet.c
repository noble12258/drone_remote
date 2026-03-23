#include "receive_packet.h"
#include "si24r1.h"
#include "my_lib.h"
#include <stdio.h>
#include <string.h>
#include "transmit_packet.h"
#include "pair_freq.h"
#include "log_lib.h"
#include "queue_lib.h"
#include <rtthread.h>
#include "task_manage.h"
#include "crc_lib.h"

#define RECEIVE_BUFF_LENGHT		256		//接收数据缓冲区长度

static S_Plane plane = {LOCK};		//飞机数据
static S_Imu imuData = {0};		//姿态数据

static uint8_t receiveBuff[RECEIVE_BUFF_LENGHT] = {0};		//接收数据缓冲区
static S_Queue receiveQueue = {0};		//接收消息队列

void ReceiveData(void);
void AnalysisHandle(void);

int AnalysisData(uint8_t data, uint8_t *receiveData);

void ReceiveHandle(void)
{
	ReceiveData();
	AnalysisHandle();
}

void ReceiveInit(void)
{
	QueueInit(&receiveQueue, receiveBuff, RECEIVE_BUFF_LENGHT);
}

void ReceiveData(void)
{
	uint8_t receiveLen = 0;
	uint8_t receiveBuf[32] = {0};
	uint8_t ret = 0;
	S_Pair pair = {STEP1};
	
	GetPairInfo(&pair);

//	LogInfo("plane.pair: %d", plane.pair);
	if(pair.status == PAIR_DONE){		//判断是否已对频
		receiveLen = NRF24L01_Read_Rx_Payload(receiveBuf);		//启动带负载的ACK包模式，一定要调用该接口，否则会判定为发送失败
//		LogInfo("receiveLen: %d", receiveLen);
		if(receiveLen > 0){		//判断是否接收到数据
			for(int i = 0; i < receiveLen; i++){
				ret = EnQueue(&receiveQueue, receiveBuf[i]);
				if(ret == 1){
					LogError("receiveQueue is full");
					break;
				}
			}
		}
	}
}

//数据包解析
void AnalysisHandle(void)
{
	int ret = -1;
	uint8_t receiveData[32] = {0};
	uint8_t data = 0;
	S_Pair pair = {STEP1};
	
	GetPairInfo(&pair);
	
	if(pair.status != PAIR_DONE){
		return;
	}
	
	for(int i = 0; i < 32; i++){		//一包数据最多32字节
		if(QueueLength(&receiveQueue) > 0){
			if(DeQueue(&receiveQueue, &data) == 1){
				LogError("dequeue fail");
				return;
			}			
			
			ret = AnalysisData(data, receiveData);
			if(ret == 0){		//解析正确
				break;
			}
		} else {
			break;
		}
	}	

	if(ret == 0){
		if(receiveData[0] == CMD_ROCKER_DATA){
			plane.lock = (Lock)receiveData[1];
			plane.signal = (Signal)receiveData[2];
			plane.power = (Power)receiveData[3];
			plane.pair = (PlanePairStatus)receiveData[4];
			plane.flyMode = (E_ControlMode)receiveData[5];
			plane.voltage = receiveData[6] << 8;
			plane.voltage = (plane.voltage + receiveData[7]) / 100;			
			plane.relativeAltitude = receiveData[8] << 8;
			plane.relativeAltitude = (plane.relativeAltitude + receiveData[9]) / 10;
		}	
		else if(receiveData[0] == CMD_ACC_CAL){			
			plane.accCalStatus = (AccCal)receiveData[1];
			plane.accCalStep = (AccCalStep)receiveData[2];
			plane.accCalDataRead = (AccCalDataRead)receiveData[3];
		}
		else if(receiveData[0] == CMD_GYRO_CAL){
			plane.gyroCalStatus = (GyroCal)receiveData[1];
		}
		else if(receiveData[0] == CMD_COMPASS_CAL){
			plane.compassCalStatus = (E_CompassCal)receiveData[1];
			plane.compassCalStep = (E_CompassCalStep)receiveData[2];
			plane.compassCalDataRead = (E_CompassCalDataRead)receiveData[3];
		}
		else if(receiveData[0] == CMD_IMU){
			int16_t temp = 0;
			
			temp = (int16_t)(receiveData[1] << 8);
			temp += (int8_t)receiveData[2];
			imuData.roll = (float)temp / 100;
			
			temp = (int16_t)(receiveData[3] << 8);
			temp += (int8_t)receiveData[4];
			imuData.pitch = (float)temp / 100;
			
			temp = (int16_t)(receiveData[5] << 8);
			temp += (int8_t)receiveData[6];
			imuData.yaw = (float)temp / 100;
		}		

	}
	
}

int AnalysisData(uint8_t data, uint8_t *receiveData)
{
	static uint8_t analysisState = 0;
	static uint16_t dataLen = 0;
	static uint8_t receiveBuf[128] = {0};
	static uint8_t bufLen = 0;
	static uint8_t crc = 0;
	
	switch(analysisState){
		case 0:
			if(data == 0xAA){
				analysisState = 1;
			} else {
				analysisState = 0;
			}
			break;
		case 1:
			dataLen = (uint16_t)data << 8;		//有效数据长度高字节
			analysisState = 2;
			break;
		case 2:
			dataLen = dataLen | data;		//有效数据长度低字节
			analysisState = 3;
			LogDebug("dataLen: %d", dataLen);
			break;
		case 3:
			LogDebug("bufLen: %d", bufLen);
			if( bufLen < dataLen){		//有效数据长度
				receiveBuf[bufLen++] = data;
				LogDebug("receiveData*: %02x", receiveBuf[bufLen - 1]);
				LogDebug("data: %2x", data);
			}
			
			if(bufLen == dataLen){
				analysisState = 4;
			}
			break;
		case 4:
			crc = data;		//crc校验位
			analysisState = 5;
			break;
		case 5:
			analysisState = 0;
			if(data == 0xAD){		//包尾
				if(crc == crc8_maxim(receiveBuf, dataLen)){
					memcpy(receiveData, receiveBuf, bufLen);
					bufLen = 0;
					return 0;
				}
			}
			bufLen = 0;
			break;
		default:
			break;
	}
	
	return -1;
}

//获取plane数据
void GetPlaneInfo(S_Plane *exPlane)
{
	StructCopy((uint8_t *)&plane, (uint8_t *)exPlane, sizeof(plane));
}

//设置plane数据
void SetPlaneInfo(const S_Plane *exPlane)
{
	StructCopy((uint8_t *)exPlane, (uint8_t *)&plane, sizeof(plane));
}

void GetImuInfo(S_Imu *exImu)
{
	memcpy(exImu, &imuData, sizeof(imuData));
}
