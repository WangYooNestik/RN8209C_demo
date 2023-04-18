#include "RN8209_read.h"
#include "app_storage_RN8209.h"
#include "RN8209_func.h"
#include "RN8209_main.h"
#include "app_storage.h"





ST_RN8209_DATA_REG RN8209_AverageData;

ST_SYSTEM System;

#define READ_REG_NUM 10

#define SAMP_TIMES 25

void RN8209_Read(void)
{
	static u32 WaitTick = 0;
	static u32 WaitTick1 = 0;
	static ST_RN8209_DATA_REG TempDataBuf[SAMP_TIMES];
	static u8 Idx = 0;
	u8 i = 0;
	u64 TempSum1[3] = {0};
	s64 TempSum2[2] = {0};
	EN_Global_Status Status[READ_REG_NUM] = {Status_Success};

	if(Tick_Timeout(&WaitTick, TIME_100MS))
	{
		Status[0] = RN8209_Read_Reg_Swap(ADDR_URMS);
		Status[1] = RN8209_Read_Reg_Swap(ADDR_IARMS);
		Status[2] = RN8209_Read_Reg_Swap(ADDR_PowerPA);
		Status[3] = RN8209_Read_Reg_Swap(ADDR_EnergyP);
		Status[4] = RN8209_Read_Reg_Swap(ADDR_IBRMS);
		Status[5] = RN8209_Read_Reg_Swap(ADDR_PowerPB);
		Status[6] = RN8209_Read_Reg_Swap(ADDR_EnergyD);
		Status[7] = RN8209_Read_Reg_Swap(ADDR_EMUStatus);

		Status[8] = RN8209_Read_Reg_Swap(ADDR_PFCnt);
		Status[9] = RN8209_Read_Reg_Swap(ADDR_DFcnt);

		for(i=0; i<READ_REG_NUM; i++)
		{
			if(Status[i] == Status_Error)
			{
				Set_RN8209_Main_Func(RN8209_INIT);
				return;
			}
		}

		if(Tick_Timeout(&WaitTick1, TIME_5S))
		{
			Storage_RN8209.DataReg.PFCnt = RN8209_RegRcv.Data.PFCnt;
			Storage_RN8209.DataReg.DFcnt = RN8209_RegRcv.Data.DFcnt;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}

		if(RN8209_RegRcv.Data.EnergyP)
		{
			Storage_RN8209.EA_Count += RN8209_RegRcv.Data.EnergyP;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}

		if(RN8209_RegRcv.Data.EnergyD)
		{
			Storage_RN8209.EB_Count += RN8209_RegRcv.Data.EnergyD;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}
#if 1
		TempDataBuf[Idx] = RN8209_RegRcv.Data;
#else
		TempDataBuf[Idx].URMS = RN8209_RegRcv.Data.URMS;
		TempDataBuf[Idx].IARMS = RN8209_RegRcv.Data.IARMS;
		TempDataBuf[Idx].PowerPA = RN8209_RegRcv.Data.PowerPA;
		TempDataBuf[Idx].IBRMS = RN8209_RegRcv.Data.IBRMS;
		TempDataBuf[Idx].PowerPB = RN8209_RegRcv.Data.PowerPB;
#endif

		if(++Idx >= SAMP_TIMES)
		{
			Idx = 0;

			//测试读数据耗时
			HAL_GPIO_TogglePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin);
		}

		for(i=0; i<SAMP_TIMES; i++)
		{
			TempSum1[0] += TempDataBuf[i].URMS;
			TempSum1[1] += TempDataBuf[i].IARMS;
			TempSum1[2] += TempDataBuf[i].IBRMS;
			TempSum2[0] += TempDataBuf[i].PowerPA;
			TempSum2[1] += TempDataBuf[i].PowerPB;
		}

		RN8209_AverageData.URMS = TempSum1[0] / SAMP_TIMES;
		RN8209_AverageData.IARMS = TempSum1[1] / SAMP_TIMES;
		RN8209_AverageData.IBRMS = TempSum1[2] / SAMP_TIMES;
		RN8209_AverageData.PowerPA = TempSum2[0] / SAMP_TIMES;
		RN8209_AverageData.PowerPB = TempSum2[1] / SAMP_TIMES;

		System.Voltage   = RN8209_AverageData.URMS * Storage_RN8209.U_Gain;
		System.Current_A = RN8209_AverageData.IARMS * Storage_RN8209.IA_Gain;
		System.Power_A   = RN8209_AverageData.PowerPA * KP_VALUE;
		System.Power_A_1 = System.Voltage * System.Current_A;
		System.Energy_A  = (double)Storage_RN8209.EA_Count / RN8209_EC;
		System.Current_B = RN8209_AverageData.IBRMS * Storage_RN8209.IB_Gain;
		System.Power_B   = RN8209_AverageData.PowerPB * KP_VALUE;
		System.Power_B_1 = System.Voltage * System.Current_B;
		System.Energy_B  = (double)Storage_RN8209.EB_Count / RN8209_EC;
	}
}


