#include "led_status.h"
#include "led.h"
#include "pair_freq.h"
#include "systick.h"
#include <rtthread.h>
#include "log_lib.h"
#include "receive_packet.h"
#include "transmit_packet.h"
#include "task_manage.h"

void LedHandle(void)
{
	S_Pair pair = {STEP1};
	S_Plane plane;
	E_SignalStatus signalStatus = SIGNAL_LOST;
		
	GetPairInfo(&pair);
	GetPlaneInfo(&plane);
	signalStatus = GetSignalStatus();
	
	if(pair.status == PAIR_NOT){
		TopLedColorSet(RED);
	}
	else if(pair.status == PAIR_START){
		TopLedBlink(CYAN, 5, 10);
	}
	else if(pair.status == PAIR_FAIL){
		TopLedColorSet(YELLOW);
	}
	else if(signalStatus == SIGNAL_LOST){
		TopLedBlink(YELLOW, 2, 10);
	}
	else if(plane.lock == UNLOCK){
		TopLedColorSet(GREEN);
	}
	else if(plane.lock == LOCK){
		TopLedColorSet(BLUE);
	}
		
}

