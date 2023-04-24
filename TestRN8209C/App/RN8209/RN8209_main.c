#include "RN8209_main.h"
#include "RN8209_init.h"
#include "RN8209_calibrate_zero.h"
#include "RN8209_calibrate_gain.h"
#include "RN8209_calibrate_offset.h"
#include "RN8209_read.h"





typedef struct{
	EN_RN8209_STATE State;
	
}ST_RN8209;

static ST_RN8209 RN8209;


EN_RN8209_STATE Get_RN8209_Main_State(void)
{
	return RN8209.State;
}

void Set_RN8209_Main_State(EN_RN8209_STATE State)
{
	if(State > RN8209_RUN)
	{
		return;
	}

	RN8209.State = State;
}

void RN8209_Handler(void)
{
	switch(RN8209.State)
	{
		case RN8209_INIT:
			RN8209_Init();
			RN8209.State = RN8209_RUN;
			break;
		case RN8209_CLB_ZERO:
			//校准基准的时候，必须要将输入接地
			if(RN8209_Calibrate_Zero_Handler())
			{
				RN8209.State = RN8209_INIT;
			}
			break;
		case RN8209_CLB_GAIN:
			if(RN8209_Calibrate_Gain_Handler())
			{
				RN8209.State = RN8209_INIT;
			}
			break;
		case RN8209_CLB_OFFSET:
			if(RN8209_Calibrate_Offset_Handler())
			{
				RN8209.State = RN8209_INIT;
			}
			break;
		default:
			break;
	}

	RN8209_Read();
}

