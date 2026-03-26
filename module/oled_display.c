#include "oled_display.h"
#include "oled.h"
#include "transmit_packet.h"
#include "pair_freq.h"
#include "systick.h"
#include "receive_packet.h"
#include "adc.h"
#include "key_handle.h"
#define DEBUG_LEVEL	DEBUG_LOG
#include "log_lib.h"
#include "remote_calibrate.h"
#include "i2c.h"
#include <rtthread.h>
#include <stdio.h>
#include <string.h>
#include "voltage_detect.h"
#include "remote.h"
#include "task_manage.h"

#define PAGE_NUM	10

typedef enum{
	MAIN = 0,
	REMOTE,
	PLANE,
	ELECTRIC,
	RUDDER,
	ARM_CALIBRATE,
	PLANE_ACC_CALIBRATE,
	PLANE_GYRO_CALIBRATE,
	PLANE_IMU,
	PLANE_COMPASS_CALIBRATE,
}E_Interface;

static rt_thread_t diplay_thread = RT_NULL;
	
static uint8_t menuPage = 0;
static S_KeyValue displayKey = {0};

static void OledMainInface(void);
static void OledDisplayPairStatus(void);
static void PlaneInterface(void);
static void RemoteInterface(void);
static void RudderIndicator(void);
static void RemoteCalibrateInterface(void);
static void RemoteElectric(void);
static void AccCalibration(void);
static void GyroCalibration(void);
static void ImuInterface(void);
static void CompassCalInterface(void);

static S_OledMenu oledMenu[PAGE_NUM] = {
	//˳��ֱ�Ϊ
	//��ǰҳ�������š��ϡ��¡����ҡ�ȷ�����˳�����ǰҳ��
	{MAIN, 0, 0, 0, 1, 0, 0, (*(OledDisplayPairStatus))},
	{REMOTE, REMOTE, ELECTRIC, MAIN, PLANE, REMOTE, REMOTE,(*(RemoteInterface))},		//ң�ؽ���
	{PLANE, PLANE, PLANE_ACC_CALIBRATE, REMOTE, PLANE,PLANE, PLANE, (*(PlaneInterface))},		//�ɻ�����
	
	{ELECTRIC, ELECTRIC, ELECTRIC, ELECTRIC, ELECTRIC, ELECTRIC, REMOTE, (*(RemoteElectric))},		//����
	{RUDDER, RUDDER, RUDDER, RUDDER, RUDDER, RUDDER, REMOTE, (*(RudderIndicator))},		//������ʾ
	
	{ARM_CALIBRATE, ARM_CALIBRATE, ARM_CALIBRATE, ARM_CALIBRATE, 
	ARM_CALIBRATE, ARM_CALIBRATE, REMOTE, (*(RemoteCalibrateInterface))},		//�г�У׼
	
	{PLANE_ACC_CALIBRATE, PLANE_GYRO_CALIBRATE, PLANE_ACC_CALIBRATE, PLANE_ACC_CALIBRATE, 
	PLANE_ACC_CALIBRATE, PLANE_ACC_CALIBRATE, PLANE, (*(AccCalibration))},		//�ɻ����ٶȼ�У׼
	
	{PLANE_GYRO_CALIBRATE, PLANE_GYRO_CALIBRATE, PLANE_GYRO_CALIBRATE, PLANE_GYRO_CALIBRATE, 
	PLANE_GYRO_CALIBRATE, PLANE_GYRO_CALIBRATE, PLANE, (*(GyroCalibration))},		//�ɻ�������У׼
	
	{PLANE_IMU, PLANE_IMU, PLANE_IMU, PLANE_IMU, 
	PLANE_IMU, PLANE_IMU, PLANE, (*(ImuInterface))},		//�ɻ���̬��ʾ
	
	{PLANE_COMPASS_CALIBRATE, PLANE_COMPASS_CALIBRATE, PLANE_COMPASS_CALIBRATE, PLANE_COMPASS_CALIBRATE, 
	PLANE_COMPASS_CALIBRATE, PLANE_COMPASS_CALIBRATE, PLANE, (*(CompassCalInterface))},		//�ɻ���̬��ʾ
};

static void diplay_thread_entry(void *param)
{	
	while(1){
		GetKeyValue(&displayKey);
		if(menuPage <= (PAGE_NUM - 1)){		//��ֹ�����±�Խ��
			
			(oledMenu[menuPage].currentP)();		//���µ�ǰ����
			
		} else {
			menuPage = MAIN;
		}
		memset((void *)&displayKey, 0, sizeof(displayKey));

		rt_thread_mdelay(30);
	}
}

void OledDisplay_Main(void)
{
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear();
	
	//�����߳�
	diplay_thread = rt_thread_create("diplay_thread", diplay_thread_entry, RT_NULL, 1024, OLED_DISPLAY_PRIORITY, 5);
	
	//�����߳�
	if (diplay_thread != RT_NULL){
		rt_thread_startup(diplay_thread);
	} else {
		LogError("diplay_thread start fail");
	}
	
}

/*��ҳ*/
static void OledMainInface(void)
{
	S_Plane plane;
	float powerVoltage = 0;
	static uint16_t pairDoneDisplayCount = 0;
//	static uint8_t clearFlag = 0;
	char displayString[6] = {0};
	static uint16_t lowPowerBlinkCount = 0;
	static uint8_t lockBlinkCount = 0;
	
	GetPlaneInfo(&plane);
	if(pairDoneDisplayCount < 100){
		pairDoneDisplayCount++;
		OledDisplayChinese(2, 4, pairDone, 4);//��Ƶ���
	}
	else if(plane.flyMode == CONTROL_MODE_ATTITUDE){
		OledDisplayChinese(2, 4, ZiTaiMoShi, 4);
	}
	else if(plane.flyMode == CONTROL_MODE_ALTITUDE){
		OledDisplayChinese(2, 4, GaoDuMoShi, 4);
	}
	else if(plane.flyMode == CONTROL_MODE_POSITION){
		OledDisplayChinese(2, 4, WeiZhiMoShi, 4);
	}
	
	if(plane.lock == LOCK){
//		if(clearFlag != 1){
//			clearFlag = 1;
//			OledClearBlock(4, 4, 12, 6);			
//		}
//		OledDisplayChinese(4, 6, lockText, 2);//����
		
		OLED_DrawBMP(12, 0, 14, 2, lockBmp);
	}
	else if(plane.lock == UNLOCKING){
//		if(clearFlag != 2){
//			clearFlag = 2;
//			OledClearBlock(4, 4, 12, 6);			
//		}
//		OledDisplayChinese(4, 5, JieSuoZhong, 3);//������
		
		lockBlinkCount++;
		if(lockBlinkCount < 5){
			OledClearBlock(12, 0, 14, 2);			
		} else {
			OLED_DrawBMP(12, 0, 14, 2, lockBmp);
			if(lockBlinkCount > 10){
				lockBlinkCount = 0;
			}
		}
		
	}
	else if(plane.lock == UNLOCK){
//		if(clearFlag != 3){
//			clearFlag = 3;
//			OledClearBlock(4, 4, 12, 6);			
//		}
//		OledDisplayChinese(4, 6, unlockText, 2);//����
		
		OLED_DrawBMP(12, 0, 14, 2, unlockBmp);
		
	}
	else if(plane.lock == LOCKING){
//		if(clearFlag != 4){
//			clearFlag = 4;
//			OledClearBlock(4, 4, 12, 6);
//		}
//		OledDisplayChinese(4, 5, SuoDingZhong, 3);//������
		
		lockBlinkCount++;
		if(lockBlinkCount < 5){
			OledClearBlock(12, 0, 14, 2);			
		} else {
			OLED_DrawBMP(12, 0, 14, 2, unlockBmp);
			if(lockBlinkCount > 10){
				lockBlinkCount = 0;
			}
		}		

	}
	
	if(plane.power == POWER_LOW){		//������
		lowPowerBlinkCount++;
		if(lowPowerBlinkCount < 10){
			OledClearBlock(14, 0, 16, 2);
		} else {
			OLED_DrawBMP(14, 0, 16, 2, lowPowerBmp);
			if(lowPowerBlinkCount > 20){
				lowPowerBlinkCount = 0;
			}
		}
		
	} else {
		OledClearBlock(14, 0, 16, 2);
	}
	
	//��ʾ�����ɵ�ĸ߶�
	OLED_DrawBMP(0, 0, 2, 2, altitudeBmp);
	snprintf(displayString, 6, ":%f", plane.relativeAltitude);
	OLED_ShowString(0, 2, displayString, 16);
	OLED_ShowChar(0, 7, 'm', 16);
	
	//ң�ص�ѹ��ʾ	
	OLED_DrawBMP(8, 6, 10, 8, remoteFigure);
	powerVoltage = GetPowerVoltage();
	snprintf(displayString, 6, ":%02f", powerVoltage);
	OLED_ShowString(6, 10, displayString, 16);
	OLED_ShowChar(6, 15, 'V', 16);
	
	//�ɻ���ѹ��ʾ
	OLED_DrawBMP(0, 6, 2, 8, uavFigure);	
	snprintf(displayString, 6, ":%02f", plane.voltage);	
	OLED_ShowString(6, 2, displayString, 16);
	OLED_ShowChar(6, 7, 'V', 16);	
}

//��������ʾ
static void OledDisplayPairStatus(void)
{
	S_Pair pair = {STEP1};
	E_SignalStatus signalStatus = SIGNAL_LOST;
	static uint8_t lastStatus = 0;
	static uint8_t lastDetectStatus = 0;
	static uint8_t lastSignaleStatus = 0;
	
	GetPairInfo(&pair);
	signalStatus = GetSignalStatus();
	
	//��������ֹ��˸
	if(lastDetectStatus != pair.detectStatus){
		lastDetectStatus = pair.detectStatus;
		OLED_Clear();
	}
	else if(lastStatus != pair.status){
		lastStatus = pair.status;
		OLED_Clear();
	}
	else if(lastSignaleStatus != signalStatus){
		lastSignaleStatus = signalStatus;
		OLED_Clear();
	}
	
	if(pair.detectStatus == DETECT_NOT && pair.status != PAIR_DONE){
		OledDisplayChinese(0, 2, NoPlaneDetected, 6);//δ��⵽�ɻ�
		OledDisplayChinese(4, 0, planeIfOn, 16);
	}
	else if(pair.detectStatus == DETECT_NORMAL && pair.status == PAIR_NOT){
		OledDisplayChinese(0, 5, pairNot, 3);//δ��Ƶ
		OledDisplayChinese(3, 3, KaiShiDuiPin, 5);		//��ʼ��Ƶ
		if(displayKey.ok == 1){
			pair.status = PAIR_START;
			SetPairInfo(SET_PAIR_STATUS, pair.status);
		}
	}
	else if(pair.status == PAIR_START){
		OledDisplayChinese(0, 5, pairing,3);//��Ƶ��
	}
	else if(pair.status == PAIR_FAIL){
		OledDisplayChinese(0,2,pairFail,4);		//��Ƶʧ��
		OledDisplayChinese(3, 0, DuiPinChongShi, 9);
		if(displayKey.ok == 1){
			pair.status = PAIR_NOT;
			SetPairInfo(SET_PAIR_STATUS, pair.status);
		}
	}
	else if(signalStatus == 0 && pair.status == PAIR_DONE){
		OledDisplayChinese(0, 4, signalLost, 4);//�źŶ�ʧ
	}
	else if(pair.status == PAIR_DONE){		//��Ƶ���
		OledMainInface();		//��ʾ�ɻ�״̬��Ϣ
	}

	if(pair.status == PAIR_DONE && displayKey.ok == 1){
		menuPage = REMOTE;
		OLED_Clear();
	}

}

//������ʾ
static void RudderIndicator(void)
{
	S_Remote remote = {0};
	
	OledDisplayChinese(0, 4, DuoLiangXianShi, 4);
	
	//����ֵ
	GetRemoteInfo(&remote);
	OledDisplayChinese(2,0,throttleText,2);
	OLED_ShowChar(2,4,':',16);
	OLED_ShowNum(2,6,(u32)(remote.throttle),3,16);
	
	//���ֵ
	OledDisplayChinese(4,0,rollText,2);
	OLED_ShowChar(4,4,':',16);
	OLED_ShowNum(4,6,(u32)(remote.roll),2,16);
	
	//����ֵ
	OledDisplayChinese(6, 0, pitchText, 2);
	OLED_ShowChar(6,4,':',16);
	OLED_ShowNum(6, 6, (u32)(remote.pit), 2, 16);
	
	//ƫ��ֵ
	OledDisplayChinese(6, 8, yawText, 2);
	OLED_ShowChar(6, 12, ':', 16);
	OLED_ShowNum(6, 14, (u32)(remote.yaw), 2, 16);
	
	if(displayKey.cancel == 1){
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}	
}

//�г�У׼
static void RemoteCalibrateInterface(void)
{
	S_RemoteCalibrate remoteCal = {CAL_NONE};
	S_Remote remote = {0};
	static u8 lastStatus = 0;
	static uint8_t lastCalState = 0;
	char targetString[10] = {0};

	OledDisplayChinese(0, 4, XingChengJiaoZhun, 4);
	
	GetCalibrateInfo(&remoteCal);
	GetRemoteInfo(&remote);
	
	if(displayKey.cancel == 1){		//��ȡ�����˳�У׼����
//		if(remoteCal.calState != CAL_DONE){
//			remoteCal.calState = CAL_NONE;
//			SetCalibrateInfo(CALIBRATE_STATE, remoteCal.calState);
//		}
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
	
	if(lastStatus != remoteCal.calStep){
		OLED_Clear();
		lastStatus = remoteCal.calStep;
	}

	if(displayKey.ok == 1 && menuPage == ARM_CALIBRATE && remoteCal.calState != CAL_START){		//��ȷ�ϼ�����У׼
		remoteCal.calState = CAL_START;
		SetCalibrateInfo(CALIBRATE_STATE, remoteCal.calState);
		OLED_Clear();
	}
	else if(displayKey.cancel == 1){		//��ȡ�����˳�У׼����
		if(remoteCal.calState != CAL_DONE){
			remoteCal.calState = CAL_NONE;
			SetCalibrateInfo(CALIBRATE_STATE, remoteCal.calState);
		}
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
	else if(remoteCal.calState == CAL_START){
		if(remoteCal.calStep == LEFT_UP){		//��ң������
			OledDisplayChinese(2, 8, ZuoYaoGan, 3);
			OledDisplayChinese(4, 10, ShangTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.throttle), 3, 16);
			OLED_ShowString(6, 6, "T:>800", 16);
		}
		else if(remoteCal.calStep == LEFT_DOWN){		//��ҡ������
			OledDisplayChinese(2, 8, ZuoYaoGan, 3);
			OledDisplayChinese(4, 10, XiaLa, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.throttle), 3, 16);
			OLED_ShowString(6, 6, "T:<200", 16);
		}
		else if(remoteCal.calStep == LEFT_LEFT){		//��ҡ������
			OledDisplayChinese(2, 8, ZuoYaoGan, 3);
			OledDisplayChinese(4, 10, ZuoTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.yaw), 3, 16);
			OLED_ShowString(6, 6, "T:>80", 16);
		}
		else if(remoteCal.calStep == LEFT_RIGHT){		//��ҡ������
			OledDisplayChinese(2, 8, ZuoYaoGan, 3);
			OledDisplayChinese(4, 10, YouTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.yaw), 3, 16);
			OLED_ShowString(6, 6, "T:<20", 16);
		}
		else if(remoteCal.calStep == RIGHT_UP){		//��ҡ������
			OledDisplayChinese(2, 8, YouYaoGan, 3);
			OledDisplayChinese(4, 10, ShangTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.pit), 3, 16);
			OLED_ShowString(6, 6, "T:<20", 16);
		}
		else if(remoteCal.calStep == RIGHT_DOWN){		//��ҡ������
			OledDisplayChinese(2, 8, YouYaoGan, 3);
			OledDisplayChinese(4, 10, XiaLa, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.pit), 3, 16);
			OLED_ShowString(6, 6, "T:>80", 16);
		}
		else if(remoteCal.calStep == RIGHT_LEFT){		//��ң������
			OledDisplayChinese(2, 8, YouYaoGan, 3);
			OledDisplayChinese(4, 10, ZuoTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.roll), 3, 16);
			OLED_ShowString(6, 6, "T:>80", 16);
		}
		else if(remoteCal.calStep == RIGHT_RIGHT){		//��ҡ������
			OledDisplayChinese(2, 8, YouYaoGan, 3);
			OledDisplayChinese(4, 10, YouTui, 2);
			OLED_ShowString(6, 0, "C:", 16);
			OLED_ShowNum(6, 2, (u32)(remote.roll), 3, 16);
			OLED_ShowString(6, 6, "T:<20", 16);
		}
		else if(remoteCal.calStep == BOTH_MIDDLE){		//����ң�л���
			OledDisplayChinese(2, 2, BaoChiYaoGanHuiZhong, 6);
			OLED_ShowString(4, 0, "T:500", 16);
			OLED_ShowString(6, 0, "YPR:50", 16);
		}
	}
	else if(remoteCal.calState == CAL_DONE && lastCalState != CAL_DONE){		//У׼���
		LogInfo("exit ARM_CALIBRATE interface");
		OledDisplayChinese(4, 4, JiaoZhunWanCheng, 4);
		delay_ms(2000);
		menuPage = oledMenu[menuPage].back;		//У׼����Զ��˳�У׼����
		OLED_Clear();
	}
	else {
		OledDisplayChinese(4, 3, KaiShiJiaoZhun, 5);
	}
	
	if(lastCalState != remoteCal.calState){
		lastCalState = remoteCal.calState;
	}
}

static void RemoteElectric(void)
{
	uint16_t voltage = 0;
	float powerVoltage = 0;
	
	//ң�ص�ѹ��ʾ
	powerVoltage = GetPowerVoltage();
	OledDisplayChinese(0, 6, DianLiang, 2);
	OledDisplayChinese(4, 0, DianYa, 2);
	OLED_ShowChar(4, 4, ':', 16);
	voltage = (uint16_t)(powerVoltage * 100);
	OLED_ShowNum(4, 5, (u32)(voltage/100), 1, 16);
	OLED_ShowChar(4, 6, '.', 16);
	
	if((voltage%100) < 10){
		OLED_ShowNum(4, 7, (u32)(0), 1, 16);
		OLED_ShowNum(4, 8, (u32)(voltage%10), 1, 16);
	} else {
		OLED_ShowNum(4, 7, (u32)(voltage%100), 2, 16);
	}
	OLED_ShowChar(4, 9, 'V', 16);
	
	if(displayKey.cancel == 1){
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
}

//ң��������
static void RemoteInterface(void)
{
	static uint8_t row = 2;
	
	OledDisplayChinese(0, 5, remoteText,3);		//ң����
	OledDisplayChinese(2, 0, DianLiang, 2);
	OledDisplayChinese(4, 0, DuoLiangXianShi, 4);
	OledDisplayChinese(6, 0, XingChengJiaoZhun, 4);
	
	if(row >= 2 && row <= 6){
		OLED_DrawBMP(14, row, 15, row+2, TriangleFigure);
	} else {
		row = 2;
	}
	
	if(displayKey.up == 1){
		LogInfo("displayKey up");
		oledMenu[menuPage].next -= 1;
		
		//���ڸ����Ҳ�����ͼ��
		row -= 2;
		OLED_Clear();
	}
	else if(displayKey.down == 1){
		LogInfo("displayKey down");
		oledMenu[menuPage].next += 1;
		
		//���ڸ����Ҳ�����ͼ��
		row += 2;
		OLED_Clear();
	}
	else if(displayKey.cancel == 1){
		menuPage = MAIN;
		OLED_Clear();
	}
	else if(displayKey.ok == 1){
		LogInfo("ok");
		menuPage = oledMenu[menuPage].next;
		OLED_Clear();
	}
	
	if(menuPage == REMOTE){
		if(oledMenu[menuPage].next < ELECTRIC || oledMenu[menuPage].next > ARM_CALIBRATE){
			oledMenu[menuPage].next = ELECTRIC;
		}
	}
	
}

//���ٶȼ�У׼
static void AccCalibration(void)
{
	S_Pair pair = {STEP1};
	E_SignalStatus signalStatus = SIGNAL_LOST;
	uint8_t sendBuff[10] = {0};
	uint8_t buffLen = 0;
	S_Plane plane;
	static uint8_t oledClear = 0;
	
	GetPairInfo(&pair);
	signalStatus = GetSignalStatus();
	GetPlaneInfo(&plane);
	
	OledDisplayChinese(0, 2, accCal, 6);
	
	sendBuff[buffLen++] = CMD_ACC_CAL;
	
	if(pair.status == PAIR_NOT){		//�ж��Ƿ��Ѷ�Ƶ
		OledDisplayChinese(2, 5, WeiDuiPin, 3);
	}
	else if(pair.status == PAIR_DONE){		//�ж϶�Ƶ�Ƿ����
		if(signalStatus == SIGNAL_LOST){		//�ж��ź�״̬
			if(oledClear != 1){
				OLED_Clear();
				oledClear = 1;
			}
			OledDisplayChinese(2, 4, LianJieDiuShi, 4);
		}
		else if(plane.lock == UNLOCK){
			if(oledClear != 2){
				OLED_Clear();
				oledClear = 2;
			}
			OledDisplayChinese(2, 2, QingSuoDingHouCaoZuo, 6);
		}
		else if(plane.lock == LOCK){		//��������ܲ���
			if(displayKey.ok == 1 && (plane.accCalStatus != ACC_CAL_START || plane.accCalStatus != ACC_CAL_DATA_READING)){
				LogInfo("start acc calibration.");				
				
				sendBuff[buffLen++] = ACC_CAL_SET_START;
			}

			GetPlaneInfo(&plane);
			if(plane.accCalStatus == ACC_CAL_START){
				if(oledClear != 3){
					oledClear = 3;
					OLED_Clear();
				}
				
				if(plane.accCalStep == ACC_CAL_STEP_UP){		//�ɻ�����
					OledDisplayChinese(3, 6, ZhengFang, 2);
				}
				else if(plane.accCalStep == ACC_CAL_STEP_DOWN){		//����
					OledDisplayChinese(3, 6, DaoFang, 2);
				}
				else if(plane.accCalStep == ACC_CAL_STEP_FORWARD){		//ǰ����
					OledDisplayChinese(3, 5, QianShuFang, 3);
				}
				else if(plane.accCalStep == ACC_CAL_STEP_BACK){		//������
					OledDisplayChinese(3, 5, HouShuFang, 3);
				}
				else if(plane.accCalStep == ACC_CAL_STEP_LEFT){		//������
					OledDisplayChinese(3, 5, ZuoShuFang, 3);
				}
				else if(plane.accCalStep == ACC_CAL_STEP_RIGHT){		//������
					OledDisplayChinese(3, 5, YouShuFang, 3);
				}

				if(displayKey.cancel == 1){					//У׼ȡ��
					LogInfo("send acc calibrate cancel.");
					oledClear = 4;
					OLED_Clear();
					
					sendBuff[buffLen++] = ACC_CAL_SET_CANCEL;		//����У׼ȡ������

				}
				
				if(plane.accCalDataRead == ACC_CAL_DATA_READING){		//���ݶ�ȡ��
					OledDisplayChinese(6, 3, ShuJuDuQuZhong, 5);
				} else {
					OledClearSpecifyRow(6, 0);
				}

			}
			else if(plane.accCalStatus == ACC_CAL_FAIL){		//У׼ʧ��
				if(oledClear != 5){
					oledClear = 5;
					OLED_Clear();
				}
				
				LogInfo("acc calibrate fail.");
				OledDisplayChinese(2, 4, JiaoZhunShiBai, 4);
				rt_thread_mdelay(3000);
				
				//У׼��ɣ��Զ��˳�����
				menuPage = oledMenu[menuPage].back;
				OLED_Clear();
			}
			else if(plane.accCalStatus == ACC_CAL_CANCEL){		//У׼ȡ��
				LogInfo("acc calibrate cancel.");
				
				oledClear = 4;
				OLED_Clear();
				
				OledDisplayChinese(4, 4, JiZhunQuXiao, 4);
				
				rt_thread_mdelay(3000);
				menuPage = oledMenu[menuPage].back;
				OLED_Clear();
			}
			else if(plane.accCalStatus == ACC_CAL_DONE){		//У׼���
					if(oledClear != 6){
						oledClear = 6;
						OLED_Clear();
					}
					
					LogInfo("acc calibrate done.");
					OledDisplayChinese(2, 4, JiaoZhunWanCheng, 4);
					rt_thread_mdelay(3000);
					
					//У׼��ɣ��Զ��˳�����
					menuPage = oledMenu[menuPage].back;
					OLED_Clear();
			}
			else if(displayKey.cancel == 1){		//δ����У׼�򷵻�
				menuPage = oledMenu[menuPage].back;
				OLED_Clear();
			}
			else {
				OledDisplayChinese(4, 3, KaiShiJiaoZhun, 5);
			}
		}
	}
	
	StoreToTransmitQueue(sendBuff, buffLen);		//���ϵĲ�ѯ���ٶ�У׼״̬
	
}

//������У׼
static void GyroCalibration(void)
{
	S_Pair pair = {STEP1};
	E_SignalStatus signalStatus = SIGNAL_LOST;
	uint8_t sendBuff[10] = {0};
	uint8_t buffLen = 0;
	S_Plane plane;
	static uint8_t oledClear = 0;
	
	OledDisplayChinese(0, 3, gyroCal, 5);
	
	GetPairInfo(&pair);
	GetPlaneInfo(&plane);
	signalStatus = GetSignalStatus();
	
	sendBuff[buffLen++] = CMD_GYRO_CAL;
	
	if(pair.status == PAIR_NOT){
		OledDisplayChinese(2, 5, WeiDuiPin, 3);
	}
	else if(pair.status == PAIR_DONE){
		if(signalStatus == SIGNAL_LOST){		//�ж��ź�״̬
			if(oledClear != 1){
				OLED_Clear();
				oledClear = 1;
			}
			OledDisplayChinese(2, 4, LianJieDiuShi, 4);
		}
		else if(plane.lock == UNLOCK){
			if(oledClear != 2){
				OLED_Clear();
				oledClear = 2;
			}
			OledDisplayChinese(3, 2, QingSuoDingHouCaoZuo, 6);
		}
		else if(plane.lock == LOCK){
			if(displayKey.ok == 1 && (plane.gyroCalStatus != GYRO_CAL_SET_START || plane.gyroCalStatus != GYRO_CAL_DATA_READING)){
				LogInfo("start gyro calibration");
				
				sendBuff[buffLen++] = GYRO_CAL_SET_START;
			}
			
			if(plane.gyroCalStatus == GYRO_CAL_DATA_READING){
				if(oledClear != 3){
					OLED_Clear();
					oledClear = 3;
				}
				OledDisplayChinese(3, 5, JiaoZhunZhong, 3);
			}
			else if(plane.gyroCalStatus == GYRO_CAL_FAIL){
				if(oledClear != 4){
					OLED_Clear();
					oledClear = 4;
				}
				OledDisplayChinese(2, 4, JiaoZhunShiBai, 4);
			}
			else if(plane.gyroCalStatus == GYRO_CAL_DONE){
				if(oledClear != 5){
					OLED_Clear();
					oledClear = 5;
				}
				OledDisplayChinese(2, 4, JiaoZhunWanCheng, 4);
				
				rt_thread_mdelay(3000);
				
				//У׼��ɣ��Զ��˳�����
				menuPage = oledMenu[menuPage].back;
				OLED_Clear();
			}
			else if(plane.gyroCalStatus == GYRO_CAL_NOT){
				if(oledClear != 6){
					OLED_Clear();
					oledClear = 6;
				}
								
				OledDisplayChinese(3, 3, WeiJiaoZhun, 5);
				OledDisplayChinese(6, 3, KaiShiJiaoZhun, 5);
			}
			else if(plane.gyroCalStatus == GYRO_CALIBREAD){
				if(oledClear != 7){
					OLED_Clear();
					oledClear = 7;
				}
				OledDisplayChinese(3, 3, YiJiaoZhun, 5);
				OledDisplayChinese(6, 3, KaiShiJiaoZhun, 5);			
			}
		}
		
	}

	StoreToTransmitQueue(sendBuff, buffLen);		//���ϵĲ�ѯ���ٶ�У׼״̬
	
	if(displayKey.cancel == 1){
//		sendBuff[buffLen++] = CMD_GYRO_CAL;
//		sendBuff[buffLen++] = GYRO_CAL_SET_CANCEL;
////		SendData(sendBuff, buffLen);

//		uint16_t i = 250;
//		while(i--){
//			GetPlaneInfo(&plane);
//			if(plane.gyroCalStatus == GYRO_CAL_CANCEL){
//				LogInfo("gyro calibrate cancel success.");
//				menuPage = oledMenu[menuPage].back;
//				OLED_Clear();
//				return;
//			}
//			rt_thread_mdelay(20);
//		}
//		LogInfo("gyro calibrate cancel fail.");
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
}

void ImuInterface(void)
{
	uint8_t sendBuff[10] = {0};
	uint8_t buffLen = 0;
	S_Imu imu = {0};
	char displayString[8] = {0};
	
	GetImuInfo(&imu);
	
	sendBuff[buffLen++] = CMD_IMU;
	
	OledDisplayChinese(0, 4, Zi1_Tai4_Xian3_Shi4, 4);
	
	OledDisplayChinese(2, 0, Heng2_Gun3, 2);
	snprintf(displayString, 8, ": %f", imu.roll);
	OLED_ShowString(2, 4, displayString, 16);
	
	OledDisplayChinese(4, 0, Fu3_Yang3, 2);
	snprintf(displayString, 8, ": %f", imu.pitch);
	OLED_ShowString(4, 4, displayString, 16);

	OledDisplayChinese(6, 0, Pian1_Hang2, 2);
	snprintf(displayString, 8, ": %f", imu.yaw);
	OLED_ShowString(6, 4, displayString, 16);
	
	StoreToTransmitQueue(sendBuff, buffLen);
	
	if(displayKey.cancel == 1){
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
}

void CompassCalInterface(void)
{
	OledDisplayChinese(0, 3, Ci2_Li4_Ji_Jiao4_Zhun3, 5);
	uint8_t sendBuff[10] = {0};
	uint8_t buffLen = 0;
	S_Pair pair = {STEP1};
	E_SignalStatus signalStatus = SIGNAL_LOST;
	S_Plane plane = {LOCK};
	static uint8_t oledClear = 0;
	
	GetPairInfo(&pair);
	signalStatus = GetSignalStatus();
	GetPlaneInfo(&plane);
	
	sendBuff[buffLen++] = CMD_COMPASS_CAL;
	
	if(pair.status == PAIR_NOT){		//�ж��Ƿ��Ѷ�Ƶ
		OledDisplayChinese(2, 5, WeiDuiPin, 3);
	}
	else if(pair.status == PAIR_DONE){		//�ж϶�Ƶ�Ƿ����
		if(signalStatus == SIGNAL_LOST){		//�ж��ź�״̬
			if(oledClear != 1){
				OLED_Clear();
				oledClear = 1;
			}
			OledDisplayChinese(2, 4, LianJieDiuShi, 4);
		}
		else if(plane.lock == UNLOCK){
			if(oledClear != 2){
				OLED_Clear();
				oledClear = 2;
			}
			OledDisplayChinese(2, 2, QingSuoDingHouCaoZuo, 6);
		}
		else if(plane.lock == LOCK){		//��������ܲ���
			if(displayKey.ok == 1 && (plane.compassCalStatus != COMPASS_CAL_START || plane.compassCalStatus != ACC_CAL_DATA_READING)){
				LogInfo("start acc calibration.");				
				
				sendBuff[buffLen++] = COMPASS_CAL_SET_START;
			}

			GetPlaneInfo(&plane);
			if(plane.compassCalStatus == COMPASS_CAL_START){
				if(oledClear != 3){
					oledClear = 3;
					OLED_Clear();
				}
				
				if(plane.compassCalStep == COMPASS_CAL_SETP_HORIZONTAL_ROTATION){		//ˮƽ��ת
					OledDisplayChinese(3, 4, Shui3_Ping2_Xuan2_Zhuan4, 4);
				}
				else if(plane.compassCalStep == COMPASS_CAL_SETP_VERTICAL_ROTATION){
					OledDisplayChinese(3, 4, Shu4_Zhi2_Xuan2_Zhuan4, 4);
				}

				if(displayKey.cancel == 1){					//У׼ȡ��
					LogInfo("compass calibrate cancel.");
					oledClear = 4;
					OLED_Clear();
					
					sendBuff[buffLen++] = CMD_COMPASS_CAL;
					sendBuff[buffLen++] = COMPASS_CAL_SET_CANCEL;
					for(int i = 0; i < 5; i++){
						GetPlaneInfo(&plane);
						if(plane.compassCalStatus != COMPASS_CAL_SET_CANCEL){		//�жϱ�־λ�����Ƿ�ɹ���ʧ�����ط�
//							SendData(sendBuff, buffLen);
							LogInfo("send compass cal cancel");
						} else {
							break;
						}
						
						rt_thread_mdelay(10);
					}
					OledDisplayChinese(4, 4, JiZhunQuXiao, 4);

					rt_thread_mdelay(3000);
					menuPage = oledMenu[menuPage].back;
					OLED_Clear();
					return;
				}
				
				if(plane.compassCalDataRead == COMPASS_CAL_DATA_READING){		//���ݶ�ȡ��
					OledDisplayChinese(6, 3, ShuJuDuQuZhong, 5);
				} else {
					OledClearSpecifyRow(6, 0);
				}

			}
			else if(plane.compassCalStatus == COMPASS_CAL_FAIL){		//У׼ʧ��
				if(oledClear != 5){
					oledClear = 5;
					OLED_Clear();
				}
				
				LogInfo("compass calibrate fail.");
				OledDisplayChinese(2, 4, JiaoZhunShiBai, 4);
				rt_thread_mdelay(3000);
				
				//У׼��ɣ��Զ��˳�����
				menuPage = oledMenu[menuPage].back;
				OLED_Clear();
			}
			else if(plane.compassCalStatus == COMPASS_CAL_DONE){		//У׼���
					if(oledClear != 6){
						oledClear = 6;
						OLED_Clear();
					}
					
					LogInfo("compass calibrate done.");
					OledDisplayChinese(2, 4, JiaoZhunWanCheng, 4);
					rt_thread_mdelay(3000);
					
					//У׼��ɣ��Զ��˳�����
					menuPage = oledMenu[menuPage].back;
					OLED_Clear();
			}
			else {
				OledDisplayChinese(4, 3, KaiShiJiaoZhun, 5);
			}
		}
	}
	
	StoreToTransmitQueue(sendBuff, buffLen);
	
	if(displayKey.cancel == 1){
		menuPage = oledMenu[menuPage].back;
		OLED_Clear();
	}
}

//�ɻ�����
static void PlaneInterface(void)
{
	static uint8_t row = 2;
	S_Pair pair = {STEP1};
	E_SignalStatus signalStatus = SIGNAL_LOST;
	static uint8_t lastSignalStatus = SIGNAL_NORMAL;
	const uint8_t page = 4;
	static uint8_t choose = 1;
	
	OledDisplayChinese(0, 6, plane, 2);		//�ɻ�
	
	GetPairInfo(&pair);
	signalStatus = GetSignalStatus();
	
	if(pair.status == PAIR_NOT){
		OledDisplayChinese(4, 5, WeiDuiPin, 3);
	}
	else if(pair.status == PAIR_DONE && signalStatus == SIGNAL_LOST){
		if(lastSignalStatus != signalStatus){
			OLED_Clear();
			lastSignalStatus = signalStatus;
		}
		OledDisplayChinese(4, 4, LianJieDiuShi, 4);
	}
	else if(pair.status == PAIR_DONE){
		
		if(choose <= 3){
			OledDisplayChinese(2, 0, accCal, 6);
			OledDisplayChinese(4, 0, gyroCal, 5);
			OledDisplayChinese(6, 0, Zi1_Tai4_Xian3_Shi4, 4);
		}
		else if(choose > 3){
			OledDisplayChinese(2, 0, gyroCal, 5);
			OledDisplayChinese(4, 0, Zi1_Tai4_Xian3_Shi4, 4);
			OledDisplayChinese(6, 0, Ci2_Li4_Ji_Jiao4_Zhun3, 5);
		}
		
		if(row >= 2 && row <= 6){
			OLED_DrawBMP(14, row, 15, row+2, TriangleFigure);
		}
		else if(choose > 3){
			OLED_DrawBMP(14, 6, 15, 8, TriangleFigure);
		}
		else{
			row = 2;
		}
		
		if(displayKey.up == 1){
			LogInfo("displayKey up");
			
			oledMenu[menuPage].next -= 1;
			
			choose--;
			if(choose == 0){
				choose = 1;
			}
			
			//���ڸ����Ҳ�����ͼ��
			row -= 2;
			OLED_Clear();
		}
		else if(displayKey.down == 1){
			LogInfo("displayKey down");
			
			oledMenu[menuPage].next += 1;
			
			choose++;			
			if(choose > page){
				choose = 1;
			}
			
			//���ڸ����Ҳ�����ͼ��
			row += 2;
			OLED_Clear();
		}
		else if(displayKey.ok == 1){
			LogInfo("ok");
			menuPage = oledMenu[menuPage].next;
			OLED_Clear();
		}
		
		if(menuPage == PLANE){
			if(oledMenu[menuPage].next < PLANE_ACC_CALIBRATE || oledMenu[menuPage].next > PLANE_COMPASS_CALIBRATE){
				oledMenu[menuPage].next = PLANE_ACC_CALIBRATE;
			}
		}
	}

	if(displayKey.cancel == 1){
		menuPage = REMOTE;
		OLED_Clear();
	}

}

