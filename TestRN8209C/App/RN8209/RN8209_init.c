#include "RN8209_init.h"
#include "RN8209_drv.h"
#include "app_storage_RN8209.h"





#define ENABLE_IB_GAIN_REG 0

static void RN8209_Init_Ctl_Reg_Variables(void)
{
	memset(&RN8209_CtlReg, 0, sizeof(RN8209_CtlReg));

	RN8209_CtlReg = Storage_RN8209.CtlReg;

	if(CH_I_GAIN == CH_I_GAIN_1)
	{
		RN8209_CtlReg.SYSCON = 0x1640;				//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益1，电流B增益1
	}
	else if(CH_I_GAIN == CH_I_GAIN_2)
	{
		RN8209_CtlReg.SYSCON = 0x1651;				//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益2，电流B增益2
	}else{
		RN8209_CtlReg.SYSCON = 0x1662;				//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益8，电流B增益4

#if ENABLE_IB_GAIN_REG
		RN8209_CtlReg.IBGain = 0x8000 - 1;			//B通道增益翻倍
#endif
	}

	RN8209_CtlReg.EMUCON = 0xd463;					//电量读后清零，直流，关闭高通滤波，使能QF,PF脉冲输出//0x5463; 				//直流，关闭高通滤波，使能QF,PF脉冲输出

	RN8209_CtlReg.HFConst = ((0x017c * CH_I_GAIN) * (I_S_Gain_1 / I_S_Gain)); //48V 50A EC=3200 通道增益CH_I_GAIN

	RN8209_CtlReg.EMUCON2 = 0x00b0; 				//更新速度加快，自定义电量为通道B电量，//0x0040;		//自定义电量为通道B电量

#if 1	//重设启动功率
	RN8209_CtlReg.PStart = 0x60;
	RN8209_CtlReg.DStart = 0x60;//0x120;
#endif
}

static void RN8209_Init_Data_Reg_Variables(void)
{
	memset(&RN8209_DataReg, 0, sizeof(RN8209_DataReg));

	RN8209_DataReg = Storage_RN8209.DataReg;
}

void RN8209_Init_Variables(void)
{
	RN8209_CheckSum = 0;

	RN8209_Init_Ctl_Reg_Variables();

	RN8209_Init_Data_Reg_Variables();
}

static void RN8209_Write_Regs(EN_RN8209_INIT_FUNC Func)
{
	if(Func >= RN8209_INIT_FUNC_MAX)
	{
		return;
	}

	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Unlock, 1);

	switch(Func)
	{
		case RN8209_RESET:
			RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Reset, 1);
			break;
		case RN8209_SET_CTL_REG:
			RN8209_Write_Reg(ADDR_SYSCON, (u8*)&RN8209_CtlReg.SYSCON, 2);
			RN8209_Write_Reg(ADDR_EMUCON, (u8*)&RN8209_CtlReg.EMUCON, 2);
			RN8209_Write_Reg(ADDR_HFConst, (u8*)&RN8209_CtlReg.HFConst, 2);
			RN8209_Write_Reg(ADDR_EMUCON2, (u8*)&RN8209_CtlReg.EMUCON2, 2);
			break;
		case RN8209_SET_POWER_START:
			RN8209_Write_Reg(ADDR_PStart, (u8*)&RN8209_CtlReg.PStart, 2);
			RN8209_Write_Reg(ADDR_DStart, (u8*)&RN8209_CtlReg.DStart, 2);
			break;
		case RN8209_SET_IB_GAIN:
			RN8209_Write_Reg(ADDR_IBGain, (u8*)&RN8209_CtlReg.IBGain, 2);
			break;
		case RN8209_SET_DC_OFFSET:
			RN8209_Write_Reg(ADDR_DCIAH, (u8*)&RN8209_CtlReg.DCIAH, 2);		//电流a直流偏差
			RN8209_Write_Reg(ADDR_DCIBH, (u8*)&RN8209_CtlReg.DCIBH, 2);		//电流b直流偏差
			RN8209_Write_Reg(ADDR_DCUH, (u8*)&RN8209_CtlReg.DCUH, 2);		//电压直流偏差
			RN8209_Write_Reg(ADDR_DCL, (u8*)&RN8209_CtlReg.DCL, 2);
			break;
		case RN8209_SET_EVD:
			RN8209_Write_Reg(ADDR_IARMSOS, (u8*)&RN8209_CtlReg.IARMSOS, 2);	//电流a有效值偏差
			RN8209_Write_Reg(ADDR_IBRMSOS, (u8*)&RN8209_CtlReg.IBRMSOS, 2);	//电流b有效值偏差
			break;
		case RN8209_SET_GAIN:
			RN8209_Write_Reg(ADDR_GPQA, (u8*)&RN8209_CtlReg.GPQA, 2);
			RN8209_Write_Reg(ADDR_GPQB, (u8*)&RN8209_CtlReg.GPQB, 2);
			break;
		case RN8209_SET_POWER_OFFSET:
			RN8209_Write_Reg(ADDR_APOSA, (u8*)&RN8209_CtlReg.APOSA, 2);
			RN8209_Write_Reg(ADDR_APOSB, (u8*)&RN8209_CtlReg.APOSB, 2);
			break;
		case RN8209_SET_ENERGY:
			RN8209_Write_Reg(ADDR_PFCnt, (u8*)&RN8209_DataReg.PFCnt, 2);
			RN8209_Write_Reg(ADDR_DFcnt, (u8*)&RN8209_DataReg.DFcnt, 2);
			break;
		default:
			break;
	}

	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Lock, 1);
}

static u16 RN8209_Calculate_CheckSum(EN_RN8209_INIT_FUNC Func)
{
	u16 CheckSum = 0;

	if(Func >= RN8209_INIT_FUNC_MAX)
	{
		return CheckSum;
	}

	switch(Func)
	{
		case RN8209_SET_CTL_REG:
			CheckSum += RN8209_CtlReg.SYSCON;
			CheckSum += RN8209_CtlReg.EMUCON;
			CheckSum += RN8209_CtlReg.HFConst;
			CheckSum += RN8209_CtlReg.EMUCON2;
			break;
		case RN8209_SET_POWER_START:
			CheckSum += RN8209_CtlReg.PStart;
			CheckSum += RN8209_CtlReg.DStart;
			break;
		case RN8209_SET_IB_GAIN:
			CheckSum += RN8209_CtlReg.IBGain;
			break;
		case RN8209_SET_DC_OFFSET:
			CheckSum += RN8209_CtlReg.DCIAH;
			CheckSum += RN8209_CtlReg.DCIBH;
			CheckSum += RN8209_CtlReg.DCUH;
			CheckSum += RN8209_CtlReg.DCL;
			break;
		case RN8209_SET_EVD:
			CheckSum += RN8209_CtlReg.IARMSOS;
			CheckSum += RN8209_CtlReg.IBRMSOS;
			break;
		case RN8209_SET_GAIN:
			CheckSum += RN8209_CtlReg.GPQA;
			CheckSum += RN8209_CtlReg.GPQB;
			break;
		case RN8209_SET_POWER_OFFSET:
			CheckSum += RN8209_CtlReg.APOSA;
			CheckSum += RN8209_CtlReg.APOSB;
			break;
		default:
			break;
	}

	return CheckSum;
}

u16 RN8209_Init_Func(EN_RN8209_INIT_FUNC Func)
{
	u16 CheckSum = 0;

	if(Func >= RN8209_INIT_FUNC_MAX)
	{
		return CheckSum;
	}

	RN8209_Write_Regs(Func);

	CheckSum = RN8209_Calculate_CheckSum(Func);

#if ENABLE_IB_GAIN_REG
	if(Func == RN8209_SET_CTL_REG)
	{
		CheckSum += RN8209_Init_Func(RN8209_SET_IB_GAIN);
	}
#endif

	return CheckSum;
}

u16 RN8209_Init(void)
{
	u16 CheckSum = 0;

	RN8209_Init_Variables();

#if 1
	CheckSum += RN8209_Init_Func(RN8209_RESET);
	CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
	CheckSum += RN8209_Init_Func(RN8209_SET_DC_OFFSET);
	CheckSum += RN8209_Init_Func(RN8209_SET_EVD);
	CheckSum += RN8209_Init_Func(RN8209_SET_GAIN);
	CheckSum += RN8209_Init_Func(RN8209_SET_POWER_OFFSET);
	CheckSum += RN8209_Init_Func(RN8209_SET_ENERGY);
	CheckSum = ~CheckSum;
#else //完整的寄存器列表
	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Unlock, 1);
	RN8209_Write_Reg(ADDR_SYSCON, (u8*)&RN8209_CtlReg.SYSCON, 2);
	RN8209_Write_Reg(ADDR_EMUCON, (u8*)&RN8209_CtlReg.EMUCON, 2);
	RN8209_Write_Reg(ADDR_HFConst, (u8*)&RN8209_CtlReg.HFConst, 2);
	RN8209_Write_Reg(ADDR_PStart, (u8*)&RN8209_CtlReg.PStart, 2);
	RN8209_Write_Reg(ADDR_DStart, (u8*)&RN8209_CtlReg.DStart, 2);
	RN8209_Write_Reg(ADDR_GPQA, (u8*)&RN8209_CtlReg.GPQA, 2);
	RN8209_Write_Reg(ADDR_GPQB, (u8*)&RN8209_CtlReg.GPQB, 2);
//	RN8209_Write_Reg(ADDR_PhsA, (u8*)&RN8209_CtlReg.PhsA, 1);
//	RN8209_Write_Reg(ADDR_PhsB, (u8*)&RN8209_CtlReg.PhsB, 1);
//	RN8209_Write_Reg(ADDR_QPHSCAL, (u8*)&RN8209_CtlReg.QPHSCAL, 2);
	RN8209_Write_Reg(ADDR_APOSA, (u8*)&RN8209_CtlReg.APOSA, 2);
	RN8209_Write_Reg(ADDR_APOSB, (u8*)&RN8209_CtlReg.APOSB, 2);
//	RN8209_Write_Reg(ADDR_RPOSA, (u8*)&RN8209_CtlReg.RPOSA, 2);
//	RN8209_Write_Reg(ADDR_RPOSB, (u8*)&RN8209_CtlReg.RPOSB, 2);
	RN8209_Write_Reg(ADDR_IARMSOS, (u8*)&RN8209_CtlReg.IARMSOS, 2);	//电流a有效值偏差
	RN8209_Write_Reg(ADDR_IBRMSOS, (u8*)&RN8209_CtlReg.IBRMSOS, 2);	//电流b有效值偏差
	RN8209_Write_Reg(ADDR_IBGain, (u8*)&RN8209_CtlReg.IBGain, 2);
//	RN8209_Write_Reg(ADDR_D2FPL, (u8*)&RN8209_CtlReg.D2FPL, 2);
//	RN8209_Write_Reg(ADDR_D2FPH, (u8*)&RN8209_CtlReg.D2FPH, 2);
	RN8209_Write_Reg(ADDR_DCIAH, (u8*)&RN8209_CtlReg.DCIAH, 2);		//电流a直流偏差
	RN8209_Write_Reg(ADDR_DCIBH, (u8*)&RN8209_CtlReg.DCIBH, 2);		//电流b直流偏差
	RN8209_Write_Reg(ADDR_DCUH, (u8*)&RN8209_CtlReg.DCUH, 2);		//电压直流偏差
	RN8209_Write_Reg(ADDR_DCL, (u8*)&RN8209_CtlReg.DCL, 2);
	RN8209_Write_Reg(ADDR_EMUCON2, (u8*)&RN8209_CtlReg.EMUCON2, 2);
	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Lock, 1);
#endif

	return CheckSum;
}


