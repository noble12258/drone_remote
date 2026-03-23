#ifndef _REMOTE_H_
#define _REMOTE_H_

#include "stm32f10x.h"

typedef enum{
	REQUEST_COMMAND = 0,
	TEMP,		//预留
}E_RemoteInfoSet;

typedef enum{
	FLY_MODE_ATTITUDE = 0,      //姿态模式
	FLY_MODE_ALTITUDE,      //定高模式
	FLY_MODE_POSITION,      //位置模式
}E_FlyMode;

typedef enum{
	EMERGENCY_LOCK_FALSE = 0,
	EMERGENCY_LOCK_TRUE,
}E_EmergencyLock;

typedef struct{
	uint16_t throttle;		//油门
	uint8_t pit;		//俯仰舵向
	uint8_t roll;		//横滚舵向
	uint8_t yaw;		//偏航舵向
	E_FlyMode flyMode;		//飞行模式
	E_EmergencyLock emergencyLock;		//紧急锁桨
}S_Remote;

void GetRemoteInfo(S_Remote *exRemote);
void SetRemoteInfo(E_RemoteInfoSet remoteInfo, S_Remote *exRemote);
void RemoteInit(void);
void RockerValueTransform(void);
void RemotePacket(void);
void RemoteHandle(void);
#endif
