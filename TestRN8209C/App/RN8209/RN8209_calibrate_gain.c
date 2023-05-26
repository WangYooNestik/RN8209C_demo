#include "RN8209_calibrate_gain.h"
#include "RN8209_read.h"
#include "RN8209_init.h"
#include "app_storage_RN8209.h"
#include "app_storage.h"
#include "RN8209_main.h"





static u16 RN8209_Calibrate_Gain_Init(void)
{
	u16 CheckSum = 0;

	CheckSum += RN8209_Init_Func(RN8209_RESET);
	CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
	CheckSum += RN8209_Init_Func(RN8209_SET_DC_OFFSET);
	CheckSum += RN8209_Init_Func(RN8209_SET_EVD);
	CheckSum = ~CheckSum;

	return CheckSum;
}

static void RN8209_Calibrate_Voltage_Gain(void)
{
	Storage_RN8209.U_Gain = RN8209_Un / RN8209_AverageData.URMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static void RN8209_Calibrate_Ch_A_Current_Gain(void)
{
	Storage_RN8209.IA_Gain = RN8209_Ib / RN8209_AverageData.IARMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static void RN8209_Calibrate_Ch_A_Power_Gain(void)
{
	u32 Temp1 = 0;
	float PA_Err = 0.0f;
	float PA_Gain = 0.0f;

	Temp1 = System.Voltage * RN8209_Ib;
//	Temp1 = RN8209_Un * RN8209_Ib;

	PA_Err = (RN8209_AverageData.PowerPA * KP_VALUE - Temp1) / Temp1;
	PA_Gain = PA_Err / (1 + PA_Err);
	PA_Gain = 0 - PA_Gain;
	if(PA_Gain > 0.0f)
	{
		RN8209_Reg.Ctl.GPQA = PA_Gain * 0x8000;
	}else{
		RN8209_Reg.Ctl.GPQA = PA_Gain * 0x8000 + 0x10000;
	}

	Storage_RN8209.CtlReg.GPQA = RN8209_Reg.Ctl.GPQA;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static void RN8209_Calibrate_Ch_B_Current_Gain(void)
{
	Storage_RN8209.IB_Gain = RN8209_Ib / RN8209_AverageData.IBRMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static void RN8209_Calibrate_Ch_B_Power_Gain(void)
{
	u32 Temp1 = 0;
	float PB_Err = 0.0f;
	float PB_Gain = 0.0f;

	Temp1 = System.Voltage * RN8209_Ib;
//	Temp1 = RN8209_Un * RN8209_Ib;

	PB_Err = (RN8209_AverageData.PowerPB * KP_VALUE - Temp1) / Temp1;
	PB_Gain = PB_Err / (1 + PB_Err);
	PB_Gain = 0 - PB_Gain;
	if(PB_Gain > 0.0f)
	{
		RN8209_Reg.Ctl.GPQB = PB_Gain * 0x8000;
	}else{
		RN8209_Reg.Ctl.GPQB = PB_Gain * 0x8000 + 0x10000;
	}

	Storage_RN8209.CtlReg.GPQB = RN8209_Reg.Ctl.GPQB;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}





typedef enum{
	RN8209_CLB_GAIN_INIT = 0,
	RN8209_CLB_GAIN_WAIT_1, 			//等待读取到新的功率值，并进行计算
	RN8209_CLB_GAIN_END,				//结束
}EN_RN8209_CLB_GAIN_STATE;

typedef struct{
	EN_RN8209_CLB_GAIN_TYPE Type;
	EN_RN8209_CLB_GAIN_STATE State;
}ST_RN8209_CLB_GAIN;

static ST_RN8209_CLB_GAIN RN8209_CalibrateGain;


static void RN8209_Init_Calibrate_Gain_State(void)
{
	RN8209_CalibrateGain.State = RN8209_CLB_GAIN_INIT;
}

void RN8209_Calibrate_Gain_Set_Type(EN_RN8209_CLB_GAIN_TYPE Type)
{
	IF_RN8209_CLB_GAIN_TYPE_OK(Type);

	Set_RN8209_Main_State(RN8209_CLB_GAIN);
	RN8209_Init_Calibrate_Gain_State();
	RN8209_CalibrateGain.Type = Type;
}

bool RN8209_Calibrate_Gain_Handler(void)
{
	bool CalibrateGainOK = false;
	static u32 WaitTick = 0;

	switch(RN8209_CalibrateGain.State)
	{
		case RN8209_CLB_GAIN_INIT:
			RN8209_CheckSum = RN8209_Calibrate_Gain_Init();
			Reset_Tick(&WaitTick);
			RN8209_CalibrateGain.State = RN8209_CLB_GAIN_WAIT_1;
			break;
		case RN8209_CLB_GAIN_WAIT_1:
			if(Tick_Timeout(&WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				if(RN8209_CalibrateGain.Type == RN8209_CLB_GAIN_VOLTAGE)
				{
					RN8209_Calibrate_Voltage_Gain();
				}
				else if(RN8209_CalibrateGain.Type == RN8209_CLB_GAIN_CH_A)
				{
					RN8209_Calibrate_Ch_A_Current_Gain();
					RN8209_Calibrate_Ch_A_Power_Gain();
				}else{
					RN8209_Calibrate_Ch_B_Current_Gain();
					RN8209_Calibrate_Ch_B_Power_Gain();
				}

				CalibrateGainOK = true;

				RN8209_CalibrateGain.State = RN8209_CLB_GAIN_END;
			}
			break;
		default:
			break;
	}

	return CalibrateGainOK;
}



