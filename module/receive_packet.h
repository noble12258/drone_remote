#ifndef _receive_packet_h
#define _receive_packet_h

#include "stm32f10x.h"

typedef enum{
	LOCK = 0,
	UNLOCKING,
	UNLOCK,
	LOCKING,
}Lock;//锁状态

typedef enum{
	PLANE_SIGNAL_LOST = 0,
	PLANE_SIGNAL_NORMAL,
}Signal;//信号状态

typedef enum{
	POWER_NORMAL = 0,
	POWER_LOW,
}Power;//电量状态

typedef enum{
	ACC_CAL_NOT = 0,
	ACC_CAL_START,
	ACC_CAL_DONE,
	ACC_CAL_FAIL,
	ACC_CAL_CANCEL,
	ACC_CALIBREAD,
}AccCal;

typedef enum{
	ACC_CAL_STEP_BEGIN = 0,
	ACC_CAL_STEP_UP,
	ACC_CAL_STEP_DOWN,
	ACC_CAL_STEP_FORWARD,
	ACC_CAL_STEP_BACK,
	ACC_CAL_STEP_LEFT,
	ACC_CAL_STEP_RIGHT,
	ACC_CAL_STEP_SAVE,
	ACC_CAL_STEP_DONE,
}AccCalStep;

typedef enum{
	ACC_CAL_DATA_READ_NOT = 0,
	ACC_CAL_DATA_READING,
}AccCalDataRead;

typedef enum{
	GYRO_CAL_NOT = 0,
	GYRO_CAL_START,
	GYRO_CAL_DATA_READING,
	GYRO_CAL_DONE,
	GYRO_CAL_FAIL,
	GYRO_CAL_CANCEL,
	GYRO_CALIBREAD,
}GyroCal;

typedef enum{
	PLANE_PAIR_NOT = 0,
	PLANE_PAIRING,
	PLANE_PAIR_DONE,
	PLANE_PAIR_FAIL,
}PlanePairStatus;

typedef enum{
    CONTROL_MODE_ATTITUDE = 0,      //姿态模式
    CONTROL_MODE_ALTITUDE,      //定高模式
    CONTROL_MODE_POSITION,      //位置模式
}E_ControlMode;

typedef enum{
	COMPASS_CAL_NOT = 0,
	COMPASS_CAL_START,
	COMPASS_CAL_DONE,
	COMPASS_CAL_FAIL,
	COMPASS_CAL_CANCEL,
	COMPASS_CALIBREAD,
}E_CompassCal;

typedef enum{
	COMPASS_CAL_SETP_BEGIN = 0,
	COMPASS_CAL_SETP_HORIZONTAL_ROTATION,		//水平旋转
	COMPASS_CAL_SETP_VERTICAL_ROTATION,		//竖直旋转
	COMPASS_CAL_SETP_SAVE,
	COMPASS_CAL_SETP_DONE,
}E_CompassCalStep;

typedef enum{
	COMPASS_CAL_DATA_READ_NOT = 0,
	COMPASS_CAL_DATA_READING,
}E_CompassCalDataRead;

typedef enum{
	ACC_CAL_SET_NOT = 0,
	ACC_CAL_SET_START,
	ACC_CAL_SET_CANCEL,
}E_PlaneAccCalSet;

typedef enum{
	GYRO_CAL_SET_NOT = 0,
	GYRO_CAL_SET_START,
	GYRO_CAL_SET_CANCEL,
}E_PlaneGyroCalSet;

typedef enum{
	COMPASS_CAL_SET_NOT = 0,
	COMPASS_CAL_SET_START,
	COMPASS_CAL_SET_CANCEL,
}E_PlaneCompassCalSet;

typedef struct{
	Lock lock;		//锁状态
	Signal signal;		//信号状态
	Power power;	//电量状态
	PlanePairStatus pair;		//配对状态
	
	AccCal accCalStatus;		//加速度计校准状态
	AccCalStep accCalStep;		//加速度校准步骤
	AccCalDataRead	accCalDataRead;		//加速度计校准数据读取状态
	
	GyroCal gyroCalStatus;		//陀螺仪校准状态
	E_ControlMode flyMode;
	
	E_CompassCal compassCalStatus;	//磁罗盘校准状态
	E_CompassCalStep compassCalStep;		//磁罗盘校准步骤
	E_CompassCalDataRead	compassCalDataRead;		//磁罗盘校准数据读取状态
	
	float voltage;		//电压值
	
	float relativeAltitude;		//相对高度
}S_Plane;

typedef struct{
	float roll;
	float pitch;
	float yaw;
}S_Imu;

void ReceivePacket(void);
void GetPlaneInfo(S_Plane *exPlane);
void SetPlaneInfo(const S_Plane *exPlane);
int AnalysisData(uint8_t data, uint8_t *receiveData);
const uint8_t *GetReceiveCmdData(void);
void ReceiveInit(void);
void ReceiveHandle(void);
void AnalysisHandle(void);
void GetImuInfo(S_Imu *exImu);
#endif
