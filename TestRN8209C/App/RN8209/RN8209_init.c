#include "RN8209_init.h"
#include "RN8209_reg.h"
#include "RN8209_drv.h"
#include "app_storage_RN8209.h"





#define ENABLE_IB_GAIN_REG 0


static void RN8209_Set_SysCtl(EN_RN8209_SF SpecialFunc)
{
	IF_RN8209_SF_OK(SpecialFunc);

	RN8209_Reg.SSC.SpecialFunc = SpecialFunc;

	RN8209_Write_Reg(ADDR_SpecialFunc);
}

static u16 RN8209_Set_IB_Gain(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.IBGain = 0x8000 - 1; 		//B通道增益翻倍

	RN8209_Write_Reg(ADDR_IBGain);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.IBGain;

	return CheckSum;
}

static u16 RN8209_Set_Ctl_Regs(void)
{
	u16 CheckSum = 0;

#if 1
	RN8209_Reg.Ctl.SYSCON.Value = 0;
	RN8209_Reg.Ctl.SYSCON.Value_B.UartBr = BAUDRATE_4800;	//这个是只读，这里只是用来校验。调整这个参数并不影响真正的baudrate，要切换baudrate只能配置管脚
	RN8209_Reg.Ctl.SYSCON.Value_B.ADC2ON = ADC_ACTIVE;		//开启电流通道B
	RN8209_Reg.Ctl.SYSCON.Value_B.PGAIB = IB_GAIN_4;		//电流B增益4
	RN8209_Reg.Ctl.SYSCON.Value_B.PGAU = U_GAIN_4;			//电压增益4
	RN8209_Reg.Ctl.SYSCON.Value_B.PGAIA = IA_GAIN_16;		//电流A增益16

	RN8209_Reg.Ctl.EMUCON.Value = 0;
	RN8209_Reg.Ctl.EMUCON.Value_B.EnergyCLR = Clearing_After_Reading;	//电量读后清零
	RN8209_Reg.Ctl.EMUCON.Value_B.HPFIBOFF = Disactive_DHPF;			//关闭高通滤波
	RN8209_Reg.Ctl.EMUCON.Value_B.QMOD = MOD_2;							//只累加正向功率
	RN8209_Reg.Ctl.EMUCON.Value_B.PMOD = MOD_2;							//只累加正向功率
	RN8209_Reg.Ctl.EMUCON.Value_B.HPFIAOFF = Disactive_DHPF;			//关闭高通滤波
	RN8209_Reg.Ctl.EMUCON.Value_B.HPFUOFF = Disactive_DHPF;				//关闭高通滤波
	RN8209_Reg.Ctl.EMUCON.Value_B.DRUN = Active_RUN;					//使能 QF 脉冲输出和自定义电能寄存器累加
	RN8209_Reg.Ctl.EMUCON.Value_B.PRUN = Active_RUN;					//使能 PF 脉冲输出和自定义电能寄存器累加

	RN8209_Reg.Ctl.EMUCON2.Value = 0;
	RN8209_Reg.Ctl.EMUCON2.Value_B.UPMODE = UPMODE_13_982Hz;
	RN8209_Reg.Ctl.EMUCON2.Value_B.D2FM = D2FM_4;				//自定义电量为通道B电量

	RN8209_Reg.Ctl.HFConst = ((0x017c * CH_I_GAIN) * (I_S_Gain_1 / I_S_Gain)); //48V 50A EC=3200 通道增益CH_I_GAIN
#else
	if(CH_I_GAIN == CH_I_GAIN_1)
	{
		RN8209_Reg.Ctl.SYSCON.Value = 0x1640;			//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益1，电流B增益1
	}
	else if(CH_I_GAIN == CH_I_GAIN_2)
	{
		RN8209_Reg.Ctl.SYSCON.Value = 0x1651;			//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益2，电流B增益2
	}else{
		RN8209_Reg.Ctl.SYSCON.Value = 0x1662;			//0x1波特率 4800，开启电流通道B，电压增益1，电流A增益8，电流B增益4
	}

	RN8209_Reg.Ctl.EMUCON.Value = 0xd463;				//电量读后清零，直流，关闭高通滤波，使能QF,PF脉冲输出//0x5463;				//直流，关闭高通滤波，使能QF,PF脉冲输出

	RN8209_Reg.Ctl.HFConst = ((0x017c * CH_I_GAIN) * (I_S_Gain_1 / I_S_Gain)); //48V 50A EC=3200 通道增益CH_I_GAIN

	RN8209_Reg.Ctl.EMUCON2.Value = 0x00b0;				//更新速度加快，自定义电量为通道B电量，//0x0040;		//自定义电量为通道B电量
#endif

	RN8209_Write_Reg(ADDR_SYSCON);
	RN8209_Write_Reg(ADDR_EMUCON);
	RN8209_Write_Reg(ADDR_EMUCON2);
	RN8209_Write_Reg(ADDR_HFConst);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.SYSCON.Value;
	CheckSum += RN8209_Reg.Ctl.EMUCON.Value;
	CheckSum += RN8209_Reg.Ctl.EMUCON2.Value;
	CheckSum += RN8209_Reg.Ctl.HFConst;

#if ENABLE_IB_GAIN_REG
	CheckSum += RN8209_Set_IB_Gain();
#endif

	return CheckSum;
}

static u16 RN8209_Set_Power_Start(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.PStart = 0x60;
	RN8209_Reg.Ctl.DStart = 0x60;//0x120;

	RN8209_Write_Reg(ADDR_PStart);
	RN8209_Write_Reg(ADDR_DStart);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.PStart;
	CheckSum += RN8209_Reg.Ctl.DStart;

	return CheckSum;
}

static u16 RN8209_Set_DC_Offset(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.DCIAH = Storage_RN8209.CtlReg.DCIAH;
	RN8209_Reg.Ctl.DCIBH = Storage_RN8209.CtlReg.DCIBH;
	RN8209_Reg.Ctl.DCUH = Storage_RN8209.CtlReg.DCUH;
	RN8209_Reg.Ctl.DCL = Storage_RN8209.CtlReg.DCL;

	RN8209_Write_Reg(ADDR_DCIAH);		//电流a直流偏差
	RN8209_Write_Reg(ADDR_DCIBH);		//电流b直流偏差
	RN8209_Write_Reg(ADDR_DCUH);		//电压直流偏差
	RN8209_Write_Reg(ADDR_DCL);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.DCIAH;
	CheckSum += RN8209_Reg.Ctl.DCIBH;
	CheckSum += RN8209_Reg.Ctl.DCUH;
	CheckSum += RN8209_Reg.Ctl.DCL;

	return CheckSum;
}

static u16 RN8209_Set_EVD(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.IARMSOS = Storage_RN8209.CtlReg.IARMSOS;
	RN8209_Reg.Ctl.IBRMSOS = Storage_RN8209.CtlReg.IBRMSOS;

	RN8209_Write_Reg(ADDR_IARMSOS);		//电流a有效值偏差
	RN8209_Write_Reg(ADDR_IBRMSOS);		//电流b有效值偏差

	//计算校验
	CheckSum += RN8209_Reg.Ctl.IARMSOS;
	CheckSum += RN8209_Reg.Ctl.IBRMSOS;

	return CheckSum;
}

static u16 RN8209_Set_Gain(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.GPQA = Storage_RN8209.CtlReg.GPQA;
	RN8209_Reg.Ctl.GPQB = Storage_RN8209.CtlReg.GPQB;

	RN8209_Write_Reg(ADDR_GPQA);
	RN8209_Write_Reg(ADDR_GPQB);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.GPQA;
	CheckSum += RN8209_Reg.Ctl.GPQB;

	return CheckSum;
}

static u16 RN8209_Set_Power_Offset(void)
{
	u16 CheckSum = 0;

	RN8209_Reg.Ctl.APOSA = Storage_RN8209.CtlReg.APOSA;
	RN8209_Reg.Ctl.APOSB = Storage_RN8209.CtlReg.APOSB;

	RN8209_Write_Reg(ADDR_APOSA);
	RN8209_Write_Reg(ADDR_APOSB);

	//计算校验
	CheckSum += RN8209_Reg.Ctl.APOSA;
	CheckSum += RN8209_Reg.Ctl.APOSB;

	return CheckSum;
}

static void RN8209_Set_Energy(void)
{
	RN8209_Reg.Data.PFCnt = Storage_RN8209.DataReg.PFCnt;
	RN8209_Reg.Data.DFcnt = Storage_RN8209.DataReg.DFcnt;

	RN8209_Write_Reg(ADDR_PFCnt);
	RN8209_Write_Reg(ADDR_DFcnt);
}

typedef enum{
	RN8209_RESET = 0,
	RN8209_SET_CTL_REG,
	RN8209_SET_POWER_START,		//启动功率
	RN8209_SET_IB_GAIN,			//电流通道B，增益
	RN8209_SET_DC_OFFSET,		//直流偏置
	RN8209_SET_EVD,				//有效值偏差
	RN8209_SET_GAIN,			//增益校准
	RN8209_SET_POWER_OFFSET,	//有功功率偏差
	RN8209_SET_ENERGY,			//写入快速脉冲
	RN8209_INIT_FUNC_MAX
}EN_RN8209_INIT_FUNC;

#define IF_RN8209_INIT_FUNC_OK(Func) if(Func >= RN8209_INIT_FUNC_MAX) return


static u16 RN8209_Init_Func(EN_RN8209_INIT_FUNC Func)
{
	u16 CheckSum = 0;

	IF_RN8209_INIT_FUNC_OK(Func) CheckSum;

	RN8209_Set_SysCtl(SF_UNLOCK);

	switch(Func)
	{
		case RN8209_RESET:
			RN8209_Set_SysCtl(SF_RESET);
			break;
		case RN8209_SET_CTL_REG:
			CheckSum = RN8209_Set_Ctl_Regs();
			break;
		case RN8209_SET_POWER_START:
			CheckSum = RN8209_Set_Power_Start();
			break;
		case RN8209_SET_IB_GAIN:
			CheckSum = RN8209_Set_IB_Gain();
			break;
		case RN8209_SET_DC_OFFSET:
			CheckSum = RN8209_Set_DC_Offset();
			break;
		case RN8209_SET_EVD:
			CheckSum = RN8209_Set_EVD();
			break;
		case RN8209_SET_GAIN:
			CheckSum = RN8209_Set_Gain();
			break;
		case RN8209_SET_POWER_OFFSET:
			CheckSum = RN8209_Set_Power_Offset();
			break;
		case RN8209_SET_ENERGY:
			RN8209_Set_Energy();
			break;
		default:
			break;
	}

	RN8209_Set_SysCtl(SF_LOCK);

	return CheckSum;
}

void RN8209_Init(EN_RN8209_INIT_TYPE Type)
{
	u16 CheckSum = 0;

	IF_RN8209_INIT_TYPE_OK(Type);

	CheckSum += RN8209_Init_Func(RN8209_RESET);

	switch(Type)
	{
		case RN8209_INIT_ALL:
			CheckSum += RN8209_Init_Func(RN8209_SET_POWER_OFFSET);
			CheckSum += RN8209_Init_Func(RN8209_SET_ENERGY);
		case RN8209_INIT_CALIBRATE_P_GAIN_OFFSET:
			CheckSum += RN8209_Init_Func(RN8209_SET_GAIN);
		case RN8209_INIT_CALIBRATE_U_I_P_GAIN:
			CheckSum += RN8209_Init_Func(RN8209_SET_EVD);
		case RN8209_INIT_CALIBRATE_EFFECTIVE_OFFSET:
			CheckSum += RN8209_Init_Func(RN8209_SET_DC_OFFSET);
		case RN8209_INIT_CALIBRATE_DC_OFFSET:
			CheckSum += RN8209_Init_Func(RN8209_SET_CTL_REG);
			CheckSum += RN8209_Init_Func(RN8209_SET_POWER_START);
			break;
		default:
			break;
	}

	RN8209_CheckSum = (~CheckSum);

#if 0 //完整的寄存器列表
	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Unlock, 1);
	RN8209_Write_Reg(ADDR_SYSCON, (u8*)&RN8209_Reg.Ctl.SYSCON, 2);
	RN8209_Write_Reg(ADDR_EMUCON, (u8*)&RN8209_Reg.Ctl.EMUCON, 2);
	RN8209_Write_Reg(ADDR_HFConst, (u8*)&RN8209_Reg.Ctl.HFConst, 2);
	RN8209_Write_Reg(ADDR_PStart, (u8*)&RN8209_Reg.Ctl.PStart, 2);
	RN8209_Write_Reg(ADDR_DStart, (u8*)&RN8209_Reg.Ctl.DStart, 2);
	RN8209_Write_Reg(ADDR_GPQA, (u8*)&RN8209_Reg.Ctl.GPQA, 2);
	RN8209_Write_Reg(ADDR_GPQB, (u8*)&RN8209_Reg.Ctl.GPQB, 2);
//	RN8209_Write_Reg(ADDR_PhsA, (u8*)&RN8209_Reg.Ctl.PhsA, 1);
//	RN8209_Write_Reg(ADDR_PhsB, (u8*)&RN8209_Reg.Ctl.PhsB, 1);
//	RN8209_Write_Reg(ADDR_QPHSCAL, (u8*)&RN8209_Reg.Ctl.QPHSCAL, 2);
	RN8209_Write_Reg(ADDR_APOSA, (u8*)&RN8209_Reg.Ctl.APOSA, 2);
	RN8209_Write_Reg(ADDR_APOSB, (u8*)&RN8209_Reg.Ctl.APOSB, 2);
//	RN8209_Write_Reg(ADDR_RPOSA, (u8*)&RN8209_Reg.Ctl.RPOSA, 2);
//	RN8209_Write_Reg(ADDR_RPOSB, (u8*)&RN8209_Reg.Ctl.RPOSB, 2);
	RN8209_Write_Reg(ADDR_IARMSOS, (u8*)&RN8209_Reg.Ctl.IARMSOS, 2);	//电流a有效值偏差
	RN8209_Write_Reg(ADDR_IBRMSOS, (u8*)&RN8209_Reg.Ctl.IBRMSOS, 2);	//电流b有效值偏差
	RN8209_Write_Reg(ADDR_IBGain, (u8*)&RN8209_Reg.Ctl.IBGain, 2);
//	RN8209_Write_Reg(ADDR_D2FPL, (u8*)&RN8209_Reg.Ctl.D2FPL, 2);
//	RN8209_Write_Reg(ADDR_D2FPH, (u8*)&RN8209_Reg.Ctl.D2FPH, 2);
	RN8209_Write_Reg(ADDR_DCIAH, (u8*)&RN8209_Reg.Ctl.DCIAH, 2);		//电流a直流偏差
	RN8209_Write_Reg(ADDR_DCIBH, (u8*)&RN8209_Reg.Ctl.DCIBH, 2);		//电流b直流偏差
	RN8209_Write_Reg(ADDR_DCUH, (u8*)&RN8209_Reg.Ctl.DCUH, 2);		//电压直流偏差
	RN8209_Write_Reg(ADDR_DCL, (u8*)&RN8209_Reg.Ctl.DCL, 2);
	RN8209_Write_Reg(ADDR_EMUCON2, (u8*)&RN8209_Reg.Ctl.EMUCON2, 2);
	RN8209_Write_Reg(ADDR_SysCtl, (u8*)&RN8209_SysCtl.Lock, 1);
#endif
}



