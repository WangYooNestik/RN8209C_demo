#include "RN8209_calibrate_offset.h"
#include "RN8209_read.h"
#include "RN8209_init.h"
#include "app_storage_RN8209.h"
#include "app_storage.h"
#include "RN8209_main.h"





static u16 RN8209_Calibrate_Offset_Init(void)
{
	u16 CheckSum = 0;

	RN8209_Init_Variables();

	CheckSum += RN8209_Init_Func(RN8209_RESET);
	CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
	CheckSum += RN8209_Init_Func(RN8209_SET_DC_OFFSET);
	CheckSum += RN8209_Init_Func(RN8209_SET_EVD);
	CheckSum += RN8209_Init_Func(RN8209_SET_GAIN);
	CheckSum = ~CheckSum;

	return CheckSum;
}

static void RN8209_Calibrate_Ch_A_Power_Offset(void)
{
	u32 TempPowerA = 0;
	s16 TempGPQA = 0;
	float PA_Offset = 0.0f;

	if(RN8209_CtlReg.GPQA > 0)
	{
		TempGPQA = (RN8209_CtlReg.GPQA / 0x8000);
	}else{
		TempGPQA = (RN8209_CtlReg.GPQA - 0x10000) / 0x8000;
	}

	TempPowerA = System.Voltage * System.Current_A;
//	TempPowerA = RN8209_Un * RN8209_Ib * 0.05f;

	PA_Offset = (TempPowerA / KP_VALUE - RN8209_AverageData.PowerPA) / (1 + TempGPQA);
	if(PA_Offset > 0.0f)
	{
		RN8209_CtlReg.APOSA = PA_Offset;
	}else{
		RN8209_CtlReg.APOSA = PA_Offset + 0x10000;
	}

	Storage_RN8209.CtlReg.APOSA = RN8209_CtlReg.APOSA;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static void RN8209_Calibrate_Ch_B_Power_Offset(void)
{
	u32 TempPowerB = 0;
	s16 TempGPQB = 0;
	float PB_Offset = 0.0f;

	if(RN8209_CtlReg.GPQB > 0)
	{
		TempGPQB = (RN8209_CtlReg.GPQB / 0x8000);
	}else{
		TempGPQB = (RN8209_CtlReg.GPQB - 0x10000) / 0x8000;
	}

	TempPowerB = System.Voltage * System.Current_B;
//	TempPowerB = RN8209_Un * RN8209_Ib * 0.05f;

	PB_Offset = (TempPowerB / KP_VALUE - RN8209_AverageData.PowerPB) / (1 + TempGPQB);
	if(PB_Offset > 0.0f)
	{
		RN8209_CtlReg.APOSB = PB_Offset;
	}else{
		RN8209_CtlReg.APOSB = PB_Offset + 0x10000;
	}

	Storage_RN8209.CtlReg.APOSB = RN8209_CtlReg.APOSB;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}





typedef enum{
	RN8209_CLB_OFFSET_INIT = 0,
	RN8209_CLB_OFFSET_WAIT_1, 			//等待读取到新的功率值，并进行计算
	RN8209_CLB_OFFSET_END,				//结束
}EN_RN8209_CLB_OFFSET_STATE;

typedef struct{
	EN_RN8209_CLB_OFFSET_TYPE Type;
	EN_RN8209_CLB_OFFSET_STATE State;
}ST_RN8209_CLB_OFFSET;

static ST_RN8209_CLB_OFFSET RN8209_CalibrateOffset;


static void RN8209_Init_Calibrate_Offset_State(void)
{
	RN8209_CalibrateOffset.State = RN8209_CLB_OFFSET_INIT;
}

void RN8209_Calibrate_Offset_Set_Type(EN_RN8209_CLB_OFFSET_TYPE Type)
{
	if(Type >= RN8209_CLB_OFFSET_MAX)
	{
		return;
	}

	Set_RN8209_Main_State(RN8209_CLB_OFFSET);
	RN8209_Init_Calibrate_Offset_State();
	RN8209_CalibrateOffset.Type = Type;
}

bool RN8209_Calibrate_Offset_Handler(void)
{
	bool CalibrateOffsetOK = false;
	static u32 WaitTick = 0;

	switch(RN8209_CalibrateOffset.State)
	{
		case RN8209_CLB_OFFSET_INIT:
			RN8209_CheckSum = RN8209_Calibrate_Offset_Init();
			Reset_Tick(&WaitTick);
			RN8209_CalibrateOffset.State = RN8209_CLB_OFFSET_WAIT_1;
			break;
		case RN8209_CLB_OFFSET_WAIT_1:
			if(Tick_Timeout(&WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				if(RN8209_CalibrateOffset.Type == RN8209_CLB_OFFSET_CH_A)
				{
					RN8209_Calibrate_Ch_A_Power_Offset();
				}else{
					RN8209_Calibrate_Ch_B_Power_Offset();
				}

				CalibrateOffsetOK = true;

				RN8209_CalibrateOffset.State = RN8209_CLB_OFFSET_END;
			}
			break;
		default:
			break;
	}

	return CalibrateOffsetOK;
}



