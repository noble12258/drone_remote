#include "remote_calibrate.h"
#include "transmit_packet.h"
#include "log_lib.h"
#include "my_lib.h"
#include "systick.h"
#include "flash.h"
#include <string.h>
#include "math_lib.h"
#include "remote.h"
#include "task_manage.h"

#define CAL_FALSH_ADDR	0x08000000 + 1024*62		//遥控校准数据存储地址

static S_RemoteCalibrate remoteCalibrate = {CAL_NONE};		//遥控校准数据

static void CalibrateWriteToFlash(uint32_t flashAddr, S_RemoteCalibrate *remoteCal);
static void CalibrateReadFromFlash(uint32_t flashAddr,S_RemoteCalibrate *remoteCal);
void RemoteCalibrateHandle(void);

void RemoteCalibrateInit(void)
{
	uint32_t ret = 0;
	static uint8_t remoteCalInitFlag = 0;
	
	ret = *(uint32_t *)(CAL_FALSH_ADDR);
	if(ret == 10 && remoteCalInitFlag == 0){
		CalibrateReadFromFlash(CAL_FALSH_ADDR, &remoteCalibrate);
		remoteCalInitFlag = 1;
	} else {
		LogError("remote calibrate init succeed fail.");
	}
	
}

void RemoteCalibrateHandle(void)
{
	S_Remote remote = {0};

	GetRemoteInfo(&remote);
	
	if(remoteCalibrate.calState == CAL_START){
		
		switch(remoteCalibrate.calStep){
			case LEFT_UP:
				if(remote.throttle > 800){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.throttle > 800){
						LogInfo("left up");
						remoteCalibrate.throttleMax = remote.throttle;
						remoteCalibrate.calStep = LEFT_DOWN;
						LogInfo("remote.throttle: %d", remote.throttle);
					}
				}
				break;
			case LEFT_DOWN:
				if(remote.throttle < 200){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.throttle < 200){
						LogInfo("left down");
						remoteCalibrate.throttleMin = remote.throttle;
						remoteCalibrate.calStep = LEFT_LEFT;
						LogInfo("remote.throttle: %d", remote.throttle);
					}
				}
				break;
			case LEFT_LEFT:
				if(remote.yaw > 80){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.yaw > 80){
						LogInfo("left left");
						remoteCalibrate.yawMax = remote.yaw;
						remoteCalibrate.calStep = LEFT_RIGHT;
						LogInfo("remote.yaw: %d", remote.yaw);
					}
				}
				break;
			case LEFT_RIGHT:
				if(remote.yaw < 20){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.yaw < 20){
						LogInfo("left right");
						remoteCalibrate.yawMin = remote.yaw;
						remoteCalibrate.calStep = RIGHT_UP;
						LogInfo("remote.yaw: %d", remote.yaw);
					}
				}
				break;
			case RIGHT_UP:
				if(remote.pit < 20){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.pit < 20){
						LogInfo("right up");
						remoteCalibrate.pitchMin = remote.pit;
						remoteCalibrate.calStep = RIGHT_DOWN;
						LogInfo("remote.pit: %d", remote.pit);
					}
				}
				break;
			case RIGHT_DOWN:
				if(remote.pit > 80){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.pit > 80){
						LogInfo("right down");
						remoteCalibrate.pitchMax = remote.pit;
						remoteCalibrate.calStep = RIGHT_LEFT;
						LogInfo("remote.pit: %d", remote.pit);
					}
				}
				break;
			case RIGHT_LEFT:
				if(remote.roll > 80){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.roll > 80){
						LogInfo("right left");
						remoteCalibrate.rollMax = remote.roll;
						remoteCalibrate.calStep = RIGHT_RIGHT;
						LogInfo("remote.roll: %d", remote.roll);
					}
				}
				break;
			case RIGHT_RIGHT:
				if(remote.roll < 20){
					delay_ms(1000);
					GetRemoteInfo(&remote);
					if(remote.roll < 20){
						LogInfo("right right");
						remoteCalibrate.rollMin = remote.roll;
						remoteCalibrate.calStep = BOTH_MIDDLE;
						LogInfo("remote.roll: %d", remote.roll);
					}
				}
				break;
			case BOTH_MIDDLE:
				if(MyAbs(remote.throttle - 500) < 30 && MyAbs(remote.yaw - 50) < 10 && MyAbs(remote.pit - 50) < 10 && MyAbs(remote.roll - 50) < 10){
					delay_ms(3000);
					GetRemoteInfo(&remote);
						LogInfo("remote.throttle: %d", remote.throttle);
						LogInfo("remote.yaw: %d", remote.yaw);
						LogInfo("remote.pitch: %d", remote.pit);
						LogInfo("remote.roll: %d", remote.roll);
				
					if(MyAbs(remote.throttle - 500) < 30 && MyAbs(remote.yaw - 50) < 10 && MyAbs(remote.pit - 50) < 10 && MyAbs(remote.roll - 50) < 10){
						remoteCalibrate.throttleMiddle = remote.throttle;
						remoteCalibrate.yawMiddle = remote.yaw;
						remoteCalibrate.pitchMiddle = remote.pit;
						remoteCalibrate.rollMiddle = remote.roll;
					
						LogInfo("remote.throttle: %d", remote.throttle);
						LogInfo("remote.yaw: %d", remote.yaw);
						LogInfo("remote.pitch: %d", remote.pit);
						LogInfo("remote.roll: %d", remote.roll);
					
						remoteCalibrate.throttle = 500.0f / (float)(remoteCalibrate.throttleMax - remoteCalibrate.throttleMiddle);
						remoteCalibrate.throttle2 = 500.0f / (float)(remoteCalibrate.throttleMiddle - remoteCalibrate.throttleMin);
					
						remoteCalibrate.yaw = 50.0f / (float)(remoteCalibrate.yawMax - remoteCalibrate.yawMiddle);
						remoteCalibrate.yaw2 = 50.0f / (float)(remoteCalibrate.yawMiddle - remoteCalibrate.yawMin);
								
						remoteCalibrate.pitch = 50.0f / (float)(remoteCalibrate.pitchMax - remoteCalibrate.pitchMiddle);
						remoteCalibrate.pitch2 = 50.0f / (float)(remoteCalibrate.pitchMiddle - remoteCalibrate.pitchMin);
								
						remoteCalibrate.roll = 50.0f / (float)(remoteCalibrate.rollMax - remoteCalibrate.rollMiddle);
						remoteCalibrate.roll2 = 50.0f / (float)(remoteCalibrate.rollMiddle - remoteCalibrate.rollMin);

						remoteCalibrate.calStep = LEFT_UP;
						remoteCalibrate.calState = CAL_DONE;
						remoteCalibrate.calFlag = CAL_FLAG_DONE;

						CalibrateWriteToFlash(CAL_FALSH_ADDR, &remoteCalibrate);

						LogInfo("remote calibrate done");

						LogInfo("remoteCal.calState: %d", remoteCalibrate.calState);
					}
				}
			default:
				break;
		}
		
	} else {
		remoteCalibrate.calStep = LEFT_UP;
	}
	
}

//从flash中读取校准数据
static void CalibrateReadFromFlash(uint32_t flashAddr,S_RemoteCalibrate *remoteCal)
{
	uint32_t arrTemp[25] = {0};
	uint8_t arrLen = 0;

	STM32_FLASH_Read(flashAddr, arrTemp, 25);
	
	if(arrTemp[arrLen++] == 10){	//除去帧头
		remoteCal->calFlag = (E_CalibrateFlag)*(uint32_t *)&arrTemp[arrLen++];
		
		remoteCal->throttleMax = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->throttleMin = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->throttleMiddle = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->throttle = *(float *)&arrTemp[arrLen++];
		remoteCal->throttle2 = *(float *)&arrTemp[arrLen++];
		
		remoteCal->yawMax = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->yawMin = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->yawMiddle = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->yaw = *(float *)&arrTemp[arrLen++];
		remoteCal->yaw2 = *(float *)&arrTemp[arrLen++];
		
		remoteCal->rollMax = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->rollMin = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->rollMiddle = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->roll = *(float *)&arrTemp[arrLen++];
		remoteCal->roll2 = *(float *)&arrTemp[arrLen++];
		
		remoteCal->pitchMax = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->pitchMin = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->pitchMiddle = *(uint32_t *)&arrTemp[arrLen++];
		remoteCal->pitch = *(float *)&arrTemp[arrLen++];
		remoteCal->pitch2 = *(float *)&arrTemp[arrLen++];
	}		

}


//往flash中写入校准数据
static void CalibrateWriteToFlash(uint32_t flashAddr, S_RemoteCalibrate *remoteCal)
{
	u8 arrLen = 0;
	uint32_t arrTemp[25] = {0};

	arrTemp[arrLen++] = 10;		//添加帧头，判断是否有数据
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->calFlag;
	
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->throttleMax;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->throttleMin;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->throttleMiddle;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->throttle;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->throttle2;
	
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->yawMax;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->yawMin;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->yawMiddle;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->yaw;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->yaw2;
	
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->rollMax;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->rollMin;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->rollMiddle;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->roll;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->roll2;
	
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->pitchMax;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->pitchMin;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->pitchMiddle;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->pitch;
	arrTemp[arrLen++] = *(uint32_t *)&remoteCal->pitch2;
	
  FlashWriteSpecifyData(flashAddr, arrTemp, arrLen);
	
}

void GetCalibrateInfo(S_RemoteCalibrate *exRemoteCal)
{
	memcpy(exRemoteCal, &remoteCalibrate, sizeof(remoteCalibrate));
}

void SetCalibrateInfo(const E_SetCalibrateList exRemoteCal, uint8_t setValue)
{
	
	switch(exRemoteCal){
		case CALIBRATE_STATE:
			remoteCalibrate.calState = (E_CalibrateState)setValue;
			break;
		case CALIBRATE_FLAG:
			remoteCalibrate.calStep = (E_CalibrateStep)setValue;
			break;
		default:
			LogError("set list no exist");
			break;
	}

}
