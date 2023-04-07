#include "RN8209_main.h"
#include "RN8209_func.h"
#include "RN8209_calibrate_zero.h"
#include "RN8209_calibrate_gain.h"
#include "RN8209_calibrate_offset.h"
#include "RN8209_read.h"





typedef struct{
	EN_RN8209_FUNC Func;
	
}ST_RN8209;

static ST_RN8209 RN8209;


EN_RN8209_FUNC Get_RN8209_Main_State(void)
{
	return RN8209.Func;
}

void Set_RN8209_Main_Func(EN_RN8209_FUNC Func)
{
	if(Func > RN8209_IDLE)
	{
		return;
	}

	RN8209.Func = Func;

	switch(Func)
	{
		case RN8209_CLB_ZERO:
			RN8209_Init_Calibrate_Zero_State();
			break;
		case RN8209_CLB_GAIN:
			RN8209_Init_Calibrate_Gain_State();
			break;
		case RN8209_CLB_OFFSET:
			RN8209_Init_Calibrate_Offset_State();
			break;
		default:
			break;
	}
}

void RN8209_Handler(void)
{
	switch(RN8209.Func)
	{
		case RN8209_INIT:
			RN8209_Init();
			RN8209.Func = RN8209_IDLE;
			break;
		case RN8209_CLB_ZERO:
			//校准基准的时候，必须要将输入接地
			if(RN8209_Calibrate_Zero_Handler())
			{
				RN8209.Func = RN8209_INIT;
			}
			break;
		case RN8209_CLB_GAIN:
			if(RN8209_Calibrate_Gain_Handler())
			{
				RN8209.Func = RN8209_INIT;
			}
			break;
		case RN8209_CLB_OFFSET:
			if(RN8209_Calibrate_Offset_Handler())
			{
				RN8209.Func = RN8209_INIT;
			}
			break;
		default:
			break;
	}

	RN8209_Read();
}

