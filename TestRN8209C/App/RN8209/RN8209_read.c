#include "RN8209_read.h"
#include "RN8209_drv.h"
#include "RN8209_init.h"
#include "RN8209_main.h"
#include "app_storage.h"
#include "app_storage_RN8209.h"
#include "gpio.h"





ST_RN8209_DATA_REG RN8209_AverageData;

ST_RN8209_ANALOG RN8209_Analog;

ST_RN8209_READ RN8209_Read;

#define CYCLE_TIME_200MS 0.2

#define FLOAT_RESOLUTION 1.0

#define READ_REG_NUM 10

void RN8209_Read_Handler(void)
{
	EN_Global_Status Status[READ_REG_NUM] = {Status_Success};
	u8 i = 0;
	u64 TempSum1[3] = {0};
	s64 TempSum2[2] = {0};
	float TempE = 0.0f;

	if(Tick_Timeout(&RN8209_Read.WaitTick, TIME_200MS))
	{
		Status[0] = RN8209_Read_Reg(ADDR_URMS);
		Status[1] = RN8209_Read_Reg(ADDR_IARMS);
		Status[2] = RN8209_Read_Reg(ADDR_PowerPA);
		Status[3] = RN8209_Read_Reg(ADDR_EnergyP);
		Status[4] = RN8209_Read_Reg(ADDR_PFCnt);
		Status[5] = RN8209_Read_Reg(ADDR_IBRMS);
		Status[6] = RN8209_Read_Reg(ADDR_PowerPB);
		Status[7] = RN8209_Read_Reg(ADDR_EnergyD);
		Status[8] = RN8209_Read_Reg(ADDR_DFcnt);
		Status[9] = RN8209_Read_Reg(ADDR_EMUStatus);

		for(i=0; i<READ_REG_NUM; i++)
		{
			if(Status[i] == Status_Error)
			{
				Set_RN8209_Main_State(RN8209_MAIN_INIT);
				return;
			}
		}

		if(Tick_Timeout(&RN8209_Read.WaitTick1, TIME_5S))
		{
			Storage_RN8209.DataReg.PFCnt = RN8209_Reg.Data.PFCnt;
			Storage_RN8209.DataReg.DFcnt = RN8209_Reg.Data.DFcnt;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}

		if(RN8209_Reg.Data.EnergyP)
		{
			Storage_RN8209.EA_Count += RN8209_Reg.Data.EnergyP;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}

		if(RN8209_Reg.Data.EnergyD)
		{
			Storage_RN8209.EB_Count += RN8209_Reg.Data.EnergyD;

			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
		}
#if 1
		RN8209_Read.DataBuf[RN8209_Read.DataIdx] = RN8209_Reg.Data;
#else
		RN8209_Read.DataBuf[RN8209_Read.DataIdx].URMS = RN8209_Reg.Data.URMS;
		RN8209_Read.DataBuf[RN8209_Read.DataIdx].IARMS = RN8209_Reg.Data.IARMS;
		RN8209_Read.DataBuf[RN8209_Read.DataIdx].PowerPA = RN8209_Reg.Data.PowerPA;
		RN8209_Read.DataBuf[RN8209_Read.DataIdx].IBRMS = RN8209_Reg.Data.IBRMS;
		RN8209_Read.DataBuf[RN8209_Read.DataIdx].PowerPB = RN8209_Reg.Data.PowerPB;
#endif

		if(++RN8209_Read.DataIdx >= SAMP_TIMES)
		{
			RN8209_Read.DataIdx = 0;

			//测试读数据耗时
			HAL_GPIO_TogglePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin);
		}

		for(i=0; i<SAMP_TIMES; i++)
		{
			TempSum1[0] += RN8209_Read.DataBuf[i].URMS;
			TempSum1[1] += RN8209_Read.DataBuf[i].IARMS;
			TempSum1[2] += RN8209_Read.DataBuf[i].IBRMS;
			TempSum2[0] += RN8209_Read.DataBuf[i].PowerPA;
			TempSum2[1] += RN8209_Read.DataBuf[i].PowerPB;
		}

		RN8209_AverageData.URMS = TempSum1[0] / SAMP_TIMES;
		RN8209_AverageData.IARMS = TempSum1[1] / SAMP_TIMES;
		RN8209_AverageData.IBRMS = TempSum1[2] / SAMP_TIMES;
		RN8209_AverageData.PowerPA = TempSum2[0] / SAMP_TIMES;
		RN8209_AverageData.PowerPB = TempSum2[1] / SAMP_TIMES;

		RN8209_Analog.U   = RN8209_AverageData.URMS * Storage_RN8209.U_Gain;
		RN8209_Analog.IA  = RN8209_AverageData.IARMS * Storage_RN8209.IA_Gain;
		RN8209_Analog.PA  = RN8209_AverageData.PowerPA * KP_VALUE;
		RN8209_Analog.EA  = (double)Storage_RN8209.EA_Count / RN8209_EC;
		RN8209_Analog.PA1 = RN8209_Reg.Data.URMS * Storage_RN8209.U_Gain * RN8209_Reg.Data.IARMS * Storage_RN8209.IA_Gain;

		RN8209_Analog.IB  = RN8209_AverageData.IBRMS * Storage_RN8209.IB_Gain;
		RN8209_Analog.PB  = RN8209_AverageData.PowerPB * KP_VALUE;
		RN8209_Analog.EB  = (double)Storage_RN8209.EB_Count / RN8209_EC;
		RN8209_Analog.PB1 = RN8209_Reg.Data.URMS * Storage_RN8209.U_Gain * RN8209_Reg.Data.IBRMS * Storage_RN8209.IB_Gain;
	}

	if(RN8209_Read.CalculateEnergyCount > 0)
	{
		RN8209_Read.CalculateEnergyCount--;

		TempE = RN8209_Analog.PA1 * CYCLE_TIME_200MS / 3600;

		Storage_RN8209.EA.Float += TempE;
		if(Storage_RN8209.EA.Float >= FLOAT_RESOLUTION)
		{
			Storage_RN8209.EA.Int   += 1;
			Storage_RN8209.EA.Float -= FLOAT_RESOLUTION;
		}

		RN8209_Analog.EA1 = (float)Storage_RN8209.EA.Int + Storage_RN8209.EA.Float;

		TempE = RN8209_Analog.PB1 * CYCLE_TIME_200MS / 3600;

		Storage_RN8209.EB.Float += TempE;
		if(Storage_RN8209.EB.Float >= FLOAT_RESOLUTION)
		{
			Storage_RN8209.EB.Int   += 1;
			Storage_RN8209.EB.Float -= FLOAT_RESOLUTION;
		}

		RN8209_Analog.EB1 = (float)Storage_RN8209.EB.Int + Storage_RN8209.EB.Float;
	}
}


