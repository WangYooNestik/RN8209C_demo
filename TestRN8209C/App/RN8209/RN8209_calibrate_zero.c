#include "RN8209_calibrate_zero.h"
#include "RN8209_read.h"
#include "RN8209_init.h"
#include "app_storage_RN8209.h"
#include "app_storage.h"
#include "RN8209_main.h"





static ST_RN8209_DATA_REG RN8209_CalibrateZeroData_1;
static ST_RN8209_DATA_REG RN8209_CalibrateZeroData_2;


static u16 RN8209_Calibrate_Zero_Init(void)
{
	u16 CheckSum = 0;

	RN8209_Init_Variables();

	CheckSum += RN8209_Init_Func(RN8209_RESET);
	CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
	CheckSum = ~CheckSum;

	return CheckSum;
}

static u16 RN8209_Calibrate_Zero_Init_1(void)
{
	u16 CheckSum = 0;

	RN8209_Init_Variables();

	CheckSum += RN8209_Init_Func(RN8209_RESET);
	CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
	CheckSum += RN8209_Init_Func(RN8209_SET_DC_OFFSET);
	CheckSum = ~CheckSum;

	return CheckSum;
}

static void RN8209_Calibrate_Zero_CALC_DC_Offset(void)
{
	RN8209_CalibrateZeroData_1 = RN8209_AverageData;

	RN8209_Reg.Ctl.DCUH = (RN8209_CalibrateZeroData_1.URMS >> 8);
	RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.URMS >> 4) & 0x0f) << 8);

	RN8209_Reg.Ctl.DCIAH = (RN8209_CalibrateZeroData_1.IARMS >> 8);
	RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.IARMS >> 4) & 0x0f) << 0);

	RN8209_Reg.Ctl.DCIBH = (RN8209_CalibrateZeroData_1.IBRMS >> 8);
	RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.IBRMS >> 4) & 0x0f) << 4);

	Storage_RN8209.CtlReg.DCUH = RN8209_Reg.Ctl.DCUH;
	Storage_RN8209.CtlReg.DCIAH = RN8209_Reg.Ctl.DCIAH;
	Storage_RN8209.CtlReg.DCIBH = RN8209_Reg.Ctl.DCIBH;
	Storage_RN8209.CtlReg.DCL = RN8209_Reg.Ctl.DCL;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

static bool RN8209_Calibrate_Zero_Check_DC_Offset(void)
{
	bool NeedResetReg = false;

	RN8209_CalibrateZeroData_2 = RN8209_AverageData;

	if(RN8209_CalibrateZeroData_1.URMS / RN8209_CalibrateZeroData_2.URMS < 1.0f)
	{
		RN8209_CalibrateZeroData_1.URMS = ~RN8209_CalibrateZeroData_1.URMS;
		RN8209_Reg.Ctl.DCUH = (RN8209_CalibrateZeroData_1.URMS >> 8);
		RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.URMS >> 4) & 0x0f) << 8);
		NeedResetReg = true;
	}

	if(RN8209_CalibrateZeroData_1.IARMS / RN8209_CalibrateZeroData_2.IARMS < 1.0f)
	{
		RN8209_CalibrateZeroData_1.IARMS = ~RN8209_CalibrateZeroData_1.IARMS;
		RN8209_Reg.Ctl.DCIAH = (RN8209_CalibrateZeroData_1.IARMS >> 8);
		RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.IARMS >> 4) & 0x0f) << 0);
		NeedResetReg = true;
	}

	if(RN8209_CalibrateZeroData_1.IBRMS / RN8209_CalibrateZeroData_2.IBRMS < 1.0f)
	{
		RN8209_CalibrateZeroData_1.IBRMS = ~RN8209_CalibrateZeroData_1.IBRMS;
		RN8209_Reg.Ctl.DCIBH = (RN8209_CalibrateZeroData_1.IBRMS >> 8);
		RN8209_Reg.Ctl.DCL |= (((RN8209_CalibrateZeroData_1.IBRMS >> 4) & 0x0f) << 4);
		NeedResetReg = true;
	}

	Storage_RN8209.CtlReg.DCUH = RN8209_Reg.Ctl.DCUH;
	Storage_RN8209.CtlReg.DCIAH = RN8209_Reg.Ctl.DCIAH;
	Storage_RN8209.CtlReg.DCIBH = RN8209_Reg.Ctl.DCIBH;
	Storage_RN8209.CtlReg.DCL = RN8209_Reg.Ctl.DCL;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);

	return NeedResetReg;
}

static void RN8209_Calibrate_Zero_CALC_Effective_Offset(void)
{
	RN8209_Reg.Ctl.IARMSOS = (~((RN8209_AverageData.IARMS * RN8209_AverageData.IARMS) / 256));

	RN8209_Reg.Ctl.IBRMSOS = (~((RN8209_AverageData.IBRMS * RN8209_AverageData.IBRMS) / 256));

	Storage_RN8209.CtlReg.IARMSOS = RN8209_Reg.Ctl.IARMSOS;
	Storage_RN8209.CtlReg.IBRMSOS = RN8209_Reg.Ctl.IBRMSOS;

	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}





typedef enum{
	RN8209_CLB_ZERO_INIT = 0,		//重新初始化
	RN8209_CLB_ZERO_WAIT_1,			//等待初始化后，所有寄存器更新完毕，获取新的采样值，并写相应的寄存器
	RN8209_CLB_ZERO_WAIT_2,			//再次等待所有寄存器更新，检查上次写入后，新的采样值有没有达到预期效果，如果没有达到，重新写入新的值
	RN8209_CLB_ZERO_WAIT_3,
	RN8209_CLB_ZERO_SET_EFCT_REG,	//设置有效值偏差寄存器
	RN8209_CLB_ZERO_END,			//结束
}EN_RN8209_CLB_ZERO_STATE;

typedef struct{
	EN_RN8209_CLB_ZERO_STATE State;
}ST_RN8209_CLB_ZERO;

static ST_RN8209_CLB_ZERO RN8209_CalibrateZero;

static void RN8209_Init_Calibrate_Zero_State(void)
{
	RN8209_CalibrateZero.State = RN8209_CLB_ZERO_INIT;
}

void RN8209_Calibrate_Zero_Set_Type(void)
{
	Set_RN8209_Main_State(RN8209_CLB_ZERO);
	RN8209_Init_Calibrate_Zero_State();
}

//校准基准的时候，必须要将输入接地
bool RN8209_Calibrate_Zero_Handler(void)
{
	bool CalibrateZeroOK = false;
	static u32 WaitTick = 0;

	switch(RN8209_CalibrateZero.State)
	{
		case RN8209_CLB_ZERO_INIT:
			RN8209_CheckSum = RN8209_Calibrate_Zero_Init();
			Reset_Tick(&WaitTick);
			RN8209_CalibrateZero.State = RN8209_CLB_ZERO_WAIT_1;
			break;
		case RN8209_CLB_ZERO_WAIT_1:
			if(Tick_Timeout(&WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_Calibrate_Zero_CALC_DC_Offset();
				RN8209_CheckSum = RN8209_Calibrate_Zero_Init_1();
				RN8209_CalibrateZero.State = RN8209_CLB_ZERO_WAIT_2;
			}
			break;
		case RN8209_CLB_ZERO_WAIT_2:
			if(Tick_Timeout(&WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				if(RN8209_Calibrate_Zero_Check_DC_Offset())
				{
					RN8209_CheckSum = RN8209_Calibrate_Zero_Init_1();
					RN8209_CalibrateZero.State = RN8209_CLB_ZERO_WAIT_3;
				}else{
					RN8209_CalibrateZero.State = RN8209_CLB_ZERO_SET_EFCT_REG;
				}
			}
			break;
		case RN8209_CLB_ZERO_WAIT_3:
			if(Tick_Timeout(&WaitTick, WAIT_DATA_REFRESH_TIME))
			{
				RN8209_CalibrateZero.State = RN8209_CLB_ZERO_SET_EFCT_REG;
			}
			break;
		case RN8209_CLB_ZERO_SET_EFCT_REG:
			RN8209_Calibrate_Zero_CALC_Effective_Offset();
			CalibrateZeroOK = true;
			RN8209_CalibrateZero.State = RN8209_CLB_ZERO_END;
			break;
		default:
			break;
	}

	return CalibrateZeroOK;
}


