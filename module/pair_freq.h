#ifndef _pair_freq_h_
#define _pair_freq_h_

#include "stm32f10x.h"

typedef enum{
	STEP1 = 0,
	STEP2,
	STEP3,
	STEP4,
	STEP5,
}PairStep;

typedef struct
{
	PairStep step;
	uint8_t addr[5];
	uint8_t freq_channel;
}PairPacket;

typedef enum{
	PAIR_NOT = 0,
	PAIR_START,
	PAIR_DONE,
	PAIR_FAIL,
}PairStatus;

typedef enum{
	DETECT_NOT = 0,
	DETECT_NORMAL,
}DetectStatus;

typedef enum{
	SET_PAIR_STATUS = 0,
	SET_PAIR_STEP,
}E_PairInfoSet;

typedef struct{
	PairStep step;		//¶ÔÆµ²½Öè
	PairStatus status;		//¶ÔÆµ×´Ì¬
	DetectStatus detectStatus;		//¼ì²â×´Ì¬
}S_Pair;

void Pair(void);
uint32_t get_chip_id(void);
void GetPairInfo(S_Pair *exPair);
void Pair_Main(void);
void SetPairInfo(E_PairInfoSet pairInfo, PairStatus value);
#endif

