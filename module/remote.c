#include "remote.h"
#include "adc.h"
#include "my_lib.h"
#include "remote_calibrate.h"
#include "compile.h"
#include <math.h>
#include "math_lib.h"
#include "key_handle.h"
#include "pair_freq.h"
#include "log_lib.h"
#include "rtthread.h"
#include "transmit_packet.h"
#include "task_manage.h"
#include <string.h>

static S_Remote remote = {0};		//ң������
static rt_mutex_t remote_mutex = RT_NULL;

void RockerValueTransform(void);
void RemotePacket(void);

void RemoteHandle(void)
{
	RockerValueTransform();
	RemotePacket();
}

void RemoteInit(void)
{
	/* ���������� */
	remote_mutex = rt_mutex_create("remote_mutex", RT_IPC_FLAG_PRIO);
	if (remote_mutex == RT_NULL)
	{
		rt_kprintf("create dynamic remote_mutex failed.\n");
		return;
	}

}

//ң��ֵ����
void RockerValueTransform(void)
{
	const uint16_t *pAdcValue = NULL;
	S_RemoteCalibrate remoteCalibrate = {CAL_NONE};
	S_SwitchValue switchValue = {0};
	
	pAdcValue = GetAdcValue();
	GetCalibrateInfo(&remoteCalibrate);
	
#if FOUR_AXIS_UAV
		//���Ųɼ�����������Ϊ 0 ~ 1000
		//����ɼ�����������Ϊ 0 ~ 100��0.02442 = 100 / 4096 * ����ֵ

	if(remoteCalibrate.calFlag == CAL_FLAG_DONE && remoteCalibrate.calState != CAL_START){
		
		rt_mutex_take(remote_mutex, RT_WAITING_FOREVER);
		
		remote.throttle = (uint16_t)((float)pAdcValue[1] * 0.24420f);
		if(remote.throttle >= 500){
			remote.throttle = (uint16_t)((float)(remote.throttle - remoteCalibrate.throttleMiddle) * remoteCalibrate.throttle + 500.0f);
		} else {
			remote.throttle = (uint16_t)((float)(remote.throttle - remoteCalibrate.throttleMin) * remoteCalibrate.throttle2);
		}
		remote.throttle = ValueLimit(remote.throttle, 0, 999);

		remote.yaw = (uint8_t)((float)pAdcValue[0] * 0.02442f);
		if(remote.yaw >= 50){
			remote.yaw = (uint8_t)((float)(remote.yaw - remoteCalibrate.yawMiddle) * remoteCalibrate.yaw + 50.0f);
		} else {
			remote.yaw = (uint8_t)((float)(remote.yaw - remoteCalibrate.yawMin) * remoteCalibrate.yaw2);
		}
		remote.yaw = (uint8_t)ValueLimit((uint16_t)remote.yaw, 0, 99);

		remote.pit = 99 - (uint8_t)((float)pAdcValue[3] * 0.02442f);
		if(remote.pit >= 50){
			remote.pit = (uint8_t)((float)(remote.pit - remoteCalibrate.pitchMiddle) * remoteCalibrate.pitch + 50.0f);
		} else {
			remote.pit = (uint8_t)((float)(remote.pit - remoteCalibrate.pitchMin) * remoteCalibrate.pitch2);
		}
		remote.pit = (uint8_t)ValueLimit((uint16_t)remote.pit, 0, 99);

		remote.roll = (uint8_t)((float)pAdcValue[2] * 0.02442f);
		if(remote.roll >= 50){
			remote.roll = (uint8_t)((float)(remote.roll - remoteCalibrate.rollMiddle) * remoteCalibrate.roll + 50.0f);
		} else {
			remote.roll = (uint8_t)((float)(remote.roll - remoteCalibrate.rollMin) * remoteCalibrate.roll2);
		}
		remote.roll = (uint8_t)ValueLimit((uint16_t)remote.roll, 0, 99);
		
		rt_mutex_release(remote_mutex);
		
	} else {
		remote.throttle = (uint16_t)(pAdcValue[1] * 0.24420f);
		remote.yaw = (uint8_t)(pAdcValue[0] * 0.02442f);
		remote.pit = 99 - (uint8_t)(pAdcValue[3] * 0.02442f);
		remote.roll = (uint8_t)(pAdcValue[2] * 0.02442f);
	}
	
	GetSwitchValue(&switchValue);
	if(switchValue.switch2 == 0 && switchValue.switch3 == 1){
		remote.flyMode = FLY_MODE_ATTITUDE;
	}
	else if(switchValue.switch2 == 0 && switchValue.switch3 == 0){
		remote.flyMode = FLY_MODE_ALTITUDE;
	}
	else if(switchValue.switch2 == 1 && switchValue.switch3 == 0){
		remote.flyMode = FLY_MODE_POSITION;
	}
	
#elif FIXED_WING_AIRCRAFT
    remote.throttle = (uint16_t)(((float)(adcData[1]))*0.24420f);
    remote.yaw = (uint8_t)(((float)(adcData[0]))*0.02442f);
    remote.pit = (uint8_t)(((float)(adcData[3]))*0.02442f);
    remote.roll = (uint8_t)(((float)(adcData[2]))*0.02442f);
#elif BRUSHLESS_FOUR_AXIS_UAV
		//���Ųɼ�����������Ϊ 4000 ~ 8000
		//����ɼ�����������Ϊ 0 ~ 400
    remote.throttle = (uint16_t)(((float)(adcData[1]))*0.9765625f + 4000);//4000~8000
    remote.yaw = (uint16_t)(((float)(adcData[0]))*0.09765625f);
    remote.pit = (uint16_t)(((float)(adcData[3]))*0.09765625f);
    remote.roll = (uint16_t)(((float)(adcData[2]))*0.09765625f);
#endif
}

//ң�����ݷ���
void RemotePacket(void)
{
	uint8_t txPacket[27] = {0};		//���ݻ�Ҫ�������ռ��5�ֽ�
	uint8_t len = 0;
	S_Pair pair;
	
	GetPairInfo(&pair);

	if(pair.status == PAIR_DONE){
		txPacket[len++] = CMD_ROCKER_DATA;
		txPacket[len++] = *((uint8_t*)&remote.throttle);		//���ŵͰ�λ
		txPacket[len++] = *(((uint8_t*)&remote.throttle)+1);		//���Ÿ߰�λ
		txPacket[len++] = remote.pit;		//��������
		txPacket[len++] = remote.roll;		//�������
		txPacket[len++] = remote.yaw;		//ƫ������
		txPacket[len++] = remote.flyMode;		//����ģʽ
		txPacket[len++] = remote.emergencyLock;		//��������
		
		StoreToTransmitQueue(txPacket, len);		//���뷢�Ͷ���
	}

}

void GetRemoteInfo(S_Remote *exRemote)
{
	if(rt_mutex_take(remote_mutex, RT_WAITING_NO) == RT_EOK){
		memcpy((void *)exRemote, (void *)&remote, sizeof(remote));
		rt_mutex_release(remote_mutex);
	}
}

void SetRemoteInfo(E_RemoteInfoSet remoteInfo, S_Remote *exRemote)
{
	switch(remoteInfo){
		case REQUEST_COMMAND:
//			remote.cmd = exRemote->cmd;
			break;
		default:
			LogError("Option does not exist, set fail");
			break;
	}
}
