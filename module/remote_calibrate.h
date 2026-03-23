#ifndef _remote_calibrate_h
#define _remote_calibrate_h

#include "stm32f10x.h"
typedef enum{
	LEFT_UP,
	LEFT_DOWN,
	LEFT_LEFT,
	LEFT_RIGHT,
	RIGHT_UP,
	RIGHT_DOWN,
	RIGHT_LEFT,
	RIGHT_RIGHT,
	BOTH_MIDDLE,
}E_CalibrateStep;

typedef enum{
	CAL_NONE,
	CAL_START,
	CAL_DONE,
}E_CalibrateState;

typedef enum{
	CAL_FLAG_NONE,
	CAL_FLAG_DONE,
}E_CalibrateFlag;

typedef enum{
	CALIBRATE_STATE,
	CALIBRATE_FLAG,
}E_SetCalibrateList;

typedef struct{
	E_CalibrateState calState;		//遥杆校准状态
	E_CalibrateStep calStep;		//遥杆校准步骤
	E_CalibrateFlag calFlag;		//摇杆校准标志位
	
	uint16_t throttleMax;		//最大值
	uint16_t throttleMin;		//最小值
	uint16_t throttleMiddle;		//中位值
	float throttle;		//高于中位值量化系数
	float throttle2;		//低于中位值量化系数
	
	uint16_t yawMax;
	uint16_t yawMin;
	uint16_t yawMiddle;
	float yaw;
	float yaw2;
	
	uint16_t rollMax;
	uint16_t rollMin;
	uint16_t rollMiddle;
	float roll;
	float roll2;
	
	uint16_t pitchMax;
	uint16_t pitchMin;
	uint16_t pitchMiddle;
	float pitch;
	float pitch2;

}S_RemoteCalibrate;

void GetCalibrateInfo(S_RemoteCalibrate *exRemoteCal);
void SetCalibrateInfo(const E_SetCalibrateList exRemoteCal, uint8_t setValue);
void RemoteCalibrateInit(void);
void RemoteCalibrateHandle(void);
void remote_calibrate_thread_entry(void *param);
#endif 
