#include "RN8209_calibrate.h"
#include "RN8209_read.h"
#include "RN8209_init.h"
#include "RN8209_main.h"
#include "app_storage.h"
#include "app_storage_RN8209.h"





typedef enum{
	RN8209_CALIBRATE_STATE_1 = 0,
	RN8209_CALIBRATE_STATE_2,
	RN8209_CALIBRATE_STATE_3,
	RN8209_CALIBRATE_STATE_4,
	RN8209_CALIBRATE_STATE_5,
	RN8209_CALIBRATE_STATE_6,
	RN8209_CALIBRATE_STATE_7,
	RN8209_CALIBRATE_STATE_8,
	
}EN_RN8209_CALIBRATE_STATE;

typedef struct{
	u32 WaitTick;
	EN_RN8209_CALIBRATE_FUNC Func;
	EN_RN8209_CALIBRATE_STATE State;
}ST_RN8209_CALIBRATE;

ST_RN8209_CALIBRATE RN8209_Calibrate;


void RN8209_Set_Calibrate_Func(EN_RN8209_CALIBRATE_FUNC Func)
{
	IF_RN8209_CALIBRATE_FUNC_OK(Func);

	RN8209_Calibrate.Func = Func;

	RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_1;

	Reset_Tick(&RN8209_Calibrate.WaitTick);

	Set_RN8209_Main_State(RN8209_MAIN_CALIBRATE);
}





static ST_RN8209_DATA_REG RN8209_Calibrate_DC_Offset_Prev;

//校准dc偏置
static void RN8209_Calibrate_DC_Offset(void)
{
	RN8209_Calibrate_DC_Offset_Prev = RN8209_AverageData;

	RN8209_Reg.Ctrl.DCUH = (RN8209_Calibrate_DC_Offset_Prev.URMS >> 8);
	RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.URMS >> 4) & 0x0f) << 8);

	RN8209_Reg.Ctrl.DCIAH = (RN8209_Calibrate_DC_Offset_Prev.IARMS >> 8);
	RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.IARMS >> 4) & 0x0f) << 0);

	RN8209_Reg.Ctrl.DCIBH = (RN8209_Calibrate_DC_Offset_Prev.IBRMS >> 8);
	RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.IBRMS >> 4) & 0x0f) << 4);

	Storage_RN8209.CtrlReg.DCUH = RN8209_Reg.Ctrl.DCUH;
	Storage_RN8209.CtrlReg.DCIAH = RN8209_Reg.Ctrl.DCIAH;
	Storage_RN8209.CtrlReg.DCIBH = RN8209_Reg.Ctrl.DCIBH;
	Storage_RN8209.CtrlReg.DCL = RN8209_Reg.Ctrl.DCL;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//dc偏置确认
static EN_Global_Status RN8209_Calibrate_Check_DC_Offset(void)
{
	EN_Global_Status Status = Status_Success;

	if(RN8209_Calibrate_DC_Offset_Prev.URMS / RN8209_AverageData.URMS < 1.0f)
	{
		RN8209_Calibrate_DC_Offset_Prev.URMS = ~RN8209_Calibrate_DC_Offset_Prev.URMS;

		RN8209_Reg.Ctrl.DCUH = (RN8209_Calibrate_DC_Offset_Prev.URMS >> 8);
		RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.URMS >> 4) & 0x0f) << 8);

		Status = Status_Error;
	}

	if(RN8209_Calibrate_DC_Offset_Prev.IARMS / RN8209_AverageData.IARMS < 1.0f)
	{
		RN8209_Calibrate_DC_Offset_Prev.IARMS = ~RN8209_Calibrate_DC_Offset_Prev.IARMS;

		RN8209_Reg.Ctrl.DCIAH = (RN8209_Calibrate_DC_Offset_Prev.IARMS >> 8);
		RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.IARMS >> 4) & 0x0f) << 0);

		Status = Status_Error;
	}

	if(RN8209_Calibrate_DC_Offset_Prev.IBRMS / RN8209_AverageData.IBRMS < 1.0f)
	{
		RN8209_Calibrate_DC_Offset_Prev.IBRMS = ~RN8209_Calibrate_DC_Offset_Prev.IBRMS;

		RN8209_Reg.Ctrl.DCIBH = (RN8209_Calibrate_DC_Offset_Prev.IBRMS >> 8);
		RN8209_Reg.Ctrl.DCL |= (((RN8209_Calibrate_DC_Offset_Prev.IBRMS >> 4) & 0x0f) << 4);

		Status = Status_Error;
	}

	Storage_RN8209.CtrlReg.DCUH = RN8209_Reg.Ctrl.DCUH;
	Storage_RN8209.CtrlReg.DCIAH = RN8209_Reg.Ctrl.DCIAH;
	Storage_RN8209.CtrlReg.DCIBH = RN8209_Reg.Ctrl.DCIBH;
	Storage_RN8209.CtrlReg.DCL = RN8209_Reg.Ctrl.DCL;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);

	return Status;
}

//校准有效值偏置
static void RN8209_Calibrate_Effective_Offset(void)
{
	s64 Temp = 0;

	Temp = RN8209_AverageData.IARMS;
	Temp *= RN8209_AverageData.IARMS;
	RN8209_Reg.Ctrl.IARMSOS = (~(Temp >> 8));

	Temp = RN8209_AverageData.IBRMS;
	Temp *= RN8209_AverageData.IBRMS;
	RN8209_Reg.Ctrl.IBRMSOS = (~(Temp >> 8));

	Storage_RN8209.CtrlReg.IARMSOS = RN8209_Reg.Ctrl.IARMSOS;
	Storage_RN8209.CtrlReg.IBRMSOS = RN8209_Reg.Ctrl.IBRMSOS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

#define RN8209_Un 48.0f		//额定电压
#define RN8209_Ib 50.0f		//额定电流

//校准电压增益
static void RN8209_Calibrate_U_Gain(void)
{
	Storage_RN8209.U_Gain = RN8209_Un / RN8209_AverageData.URMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道a电流增益
static void RN8209_Calibrate_IA_Gain(void)
{
	Storage_RN8209.IA_Gain = RN8209_Ib / RN8209_AverageData.IARMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道a功率增益
static void RN8209_Calibrate_PA_Gain(void)
{
	u32 Temp1 = 0;
	float PA_Err = 0.0f;
	float PA_Gain = 0.0f;

	Temp1 = RN8209_Analog.Voltage * RN8209_Ib;
//	Temp1 = RN8209_Un * RN8209_Ib;

	PA_Err = (RN8209_AverageData.PowerPA * KP_VALUE - Temp1) / Temp1;
	PA_Gain = PA_Err / (1 + PA_Err);
	PA_Gain = 0 - PA_Gain;
	if(PA_Gain > 0.0f)
	{
		RN8209_Reg.Ctrl.GPQA = PA_Gain * 0x8000;
	}else{
		RN8209_Reg.Ctrl.GPQA = PA_Gain * 0x8000 + 0x10000;
	}

	Storage_RN8209.CtrlReg.GPQA = RN8209_Reg.Ctrl.GPQA;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道b电流增益
static void RN8209_Calibrate_IB_Gain(void)
{
	Storage_RN8209.IB_Gain = RN8209_Ib / RN8209_AverageData.IBRMS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道b功率增益
static void RN8209_Calibrate_PB_Gain(void)
{
	u32 Temp1 = 0;
	float PB_Err = 0.0f;
	float PB_Gain = 0.0f;

	Temp1 = RN8209_Analog.Voltage * RN8209_Ib;
//	Temp1 = RN8209_Un * RN8209_Ib;

	PB_Err = (RN8209_AverageData.PowerPB * KP_VALUE - Temp1) / Temp1;
	PB_Gain = PB_Err / (1 + PB_Err);
	PB_Gain = 0 - PB_Gain;
	if(PB_Gain > 0.0f)
	{
		RN8209_Reg.Ctrl.GPQB = PB_Gain * 0x8000;
	}else{
		RN8209_Reg.Ctrl.GPQB = PB_Gain * 0x8000 + 0x10000;
	}

	Storage_RN8209.CtrlReg.GPQB = RN8209_Reg.Ctrl.GPQB;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道a功率偏置
static void RN8209_Calibrate_PA_Gain_Offset(void)
{
	u32 TempPowerA = 0;
	s16 TempGPQA = 0;
	float PA_Offset = 0.0f;

	if(RN8209_Reg.Ctrl.GPQA > 0)
	{
		TempGPQA = (RN8209_Reg.Ctrl.GPQA / 0x8000);
	}else{
		TempGPQA = (RN8209_Reg.Ctrl.GPQA - 0x10000) / 0x8000;
	}

	TempPowerA = RN8209_Analog.Voltage * RN8209_Analog.Current_A;
//	TempPowerA = RN8209_Un * RN8209_Ib * 0.05f;

	PA_Offset = (TempPowerA / KP_VALUE - RN8209_AverageData.PowerPA) / (1 + TempGPQA);
	if(PA_Offset > 0.0f)
	{
		RN8209_Reg.Ctrl.APOSA = PA_Offset;
	}else{
		RN8209_Reg.Ctrl.APOSA = PA_Offset + 0x10000;
	}

	Storage_RN8209.CtrlReg.APOSA = RN8209_Reg.Ctrl.APOSA;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

//校准通道b功率偏置
static void RN8209_Calibrate_PB_Gain_Offset(void)
{
	u32 TempPowerB = 0;
	s16 TempGPQB = 0;
	float PB_Offset = 0.0f;

	if(RN8209_Reg.Ctrl.GPQB > 0)
	{
		TempGPQB = (RN8209_Reg.Ctrl.GPQB / 0x8000);
	}else{
		TempGPQB = (RN8209_Reg.Ctrl.GPQB - 0x10000) / 0x8000;
	}

	TempPowerB = RN8209_Analog.Voltage * RN8209_Analog.Current_B;
//	TempPowerB = RN8209_Un * RN8209_Ib * 0.05f;

	PB_Offset = (TempPowerB / KP_VALUE - RN8209_AverageData.PowerPB) / (1 + TempGPQB);
	if(PB_Offset > 0.0f)
	{
		RN8209_Reg.Ctrl.APOSB = PB_Offset;
	}else{
		RN8209_Reg.Ctrl.APOSB = PB_Offset + 0x10000;
	}

	Storage_RN8209.CtrlReg.APOSB = RN8209_Reg.Ctrl.APOSB;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}






static EN_Global_Status RN8209_Calibrate_Zero(void)
{
	EN_Global_Status Status = Status_Wait;

	switch(RN8209_Calibrate.State)
	{
		case RN8209_CALIBRATE_STATE_1:
			RN8209_Init(RN8209_INIT_CALIBRATE_DC_OFFSET);

			RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_2;
			break;
		case RN8209_CALIBRATE_STATE_2:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_DC_Offset();

				RN8209_Init(RN8209_INIT_CALIBRATE_EFFECTIVE_OFFSET);

				RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_3;
			}
			break;
		case RN8209_CALIBRATE_STATE_3:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				if(RN8209_Calibrate_Check_DC_Offset() == Status_Success)
				{
					RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_5;
				}else{
					RN8209_Init(RN8209_INIT_CALIBRATE_EFFECTIVE_OFFSET);

					RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_4;
				}
			}
			break;
		case RN8209_CALIBRATE_STATE_4:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_5;
			}
			break;
		case RN8209_CALIBRATE_STATE_5:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_Effective_Offset();

				Status = Status_Success;
			}
			break;
		default:
			break;
	}

	return Status;
}

static EN_Global_Status RN8209_Calibrate_U(void)
{
	EN_Global_Status Status = Status_Wait;

	switch(RN8209_Calibrate.State)
	{
		case RN8209_CALIBRATE_STATE_1:
			RN8209_Init(RN8209_INIT_CALIBRATE_U_I_P_GAIN);

			RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_2;
			break;
		case RN8209_CALIBRATE_STATE_2:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_U_Gain();

				Status = Status_Success;
			}
			break;
		default:
			break;
	}

	return Status;
}

static EN_Global_Status RN8209_Calibrate_I_And_P(void)
{
	EN_Global_Status Status = Status_Wait;

	switch(RN8209_Calibrate.State)
	{
		case RN8209_CALIBRATE_STATE_1:
			RN8209_Init(RN8209_INIT_CALIBRATE_U_I_P_GAIN);

			RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_2;
			break;
		case RN8209_CALIBRATE_STATE_2:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_IA_Gain();
				RN8209_Calibrate_PA_Gain();
				RN8209_Calibrate_IB_Gain();
				RN8209_Calibrate_PB_Gain();

				Status = Status_Success;
			}
			break;
		default:
			break;
	}

	return Status;
}

static EN_Global_Status RN8209_Calibrate_P_Offset(void)
{
	EN_Global_Status Status = Status_Wait;

	switch(RN8209_Calibrate.State)
	{
		case RN8209_CALIBRATE_STATE_1:
			RN8209_Init(RN8209_INIT_CALIBRATE_P_GAIN_OFFSET);

			RN8209_Calibrate.State = RN8209_CALIBRATE_STATE_2;
			break;
		case RN8209_CALIBRATE_STATE_2:
			if(Tick_Timeout(&RN8209_Calibrate.WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_PA_Gain_Offset();
				RN8209_Calibrate_PB_Gain_Offset();

				Status = Status_Success;
			}
			break;
		default:
			break;
	}

	return Status;
}





EN_Global_Status RN8209_Calibrate_Handler(void)
{
	EN_Global_Status Status = Status_Wait;

	switch(RN8209_Calibrate.Func)
	{
		case RN8209_CALIBRATE_FUNC_ZERO:
			Status = RN8209_Calibrate_Zero();
			break;
		case RN8209_CALIBRATE_FUNC_U_GAIN:
			Status = RN8209_Calibrate_U();
			break;
		case RN8209_CALIBRATE_FUNC_I_AND_P_GAIN:
			Status = RN8209_Calibrate_I_And_P();
			break;
		case RN8209_CALIBRATE_FUNC_P_GAIN_OFFSET:
			Status = RN8209_Calibrate_P_Offset();
			break;
		default:
			break;
	}

	return Status;
}


