#include "RN8209_drv.h"
#include "app_tool.h"
#include "RN8209_func.h"





//用于计算校验
u16 RN8209_CheckSum = 0;

//用于主动修改，让后应用到程序中
ST_RN8209_CTL_REG RN8209_CtlReg = {0};

//用于写PFCnt和DFCnt
ST_RN8209_DATA_REG RN8209_DataReg = {0};

//用于接收获取到的，当前RN8209上的实际的值
ST_RN8209_REG RN8209_RegRcv = {0};

//特殊功能命令，抽象为一个地址为0xea的寄存器，通过给这个寄存器写不同的值，实现不同的控制功能
const ST_RN8209_SYSCTL RN8209_SysCtl = {.Unlock = UNLOCK, LOCK, SLC_CH_A, SLC_CH_B, RST};





static bool If_Found_RN8209_Reg(EN_RN8209_REG_ADDR RegAddr)
{
	bool FoundReg = false;

	switch(RegAddr)
	{
		case ADDR_SYSCON:
		case ADDR_EMUCON:
		case ADDR_HFConst:
		case ADDR_PStart:
		case ADDR_DStart:
		case ADDR_GPQA:
		case ADDR_GPQB:
		case ADDR_PhsA:
		case ADDR_PhsB:
		case ADDR_QPHSCAL:
		case ADDR_APOSA:
		case ADDR_APOSB:
		case ADDR_RPOSA:
		case ADDR_RPOSB:
		case ADDR_IARMSOS:
		case ADDR_IBRMSOS:
		case ADDR_IBGain:
		case ADDR_D2FPL:
		case ADDR_D2FPH:
		case ADDR_DCIAH:
		case ADDR_DCIBH:
		case ADDR_DCUH:
		case ADDR_DCL:
		case ADDR_EMUCON2:
		case ADDR_PFCnt:
		case ADDR_DFcnt:
		case ADDR_IARMS:
		case ADDR_IBRMS:
		case ADDR_URMS:
		case ADDR_UFreq:
		case ADDR_PowerPA:
		case ADDR_PowerPB:
		case ADDR_PowerQ:
		case ADDR_EnergyP:
		case ADDR_EnergyP2:
		case ADDR_EnergyD:
		case ADDR_EnergyD2:
		case ADDR_EMUStatus:
		case ADDR_SPL_IA:
		case ADDR_SPL_IB:
		case ADDR_SPL_U:
		case ADDR_UFreq2:
		case ADDR_IE:
		case ADDR_IF:
		case ADDR_RIF:
		case ADDR_SysStatus:
		case ADDR_RData:
		case ADDR_WData:
		case ADDR_DeviceID:
		case ADDR_SysCtl:
			FoundReg =  true;
			break;
		default:
			break;
	}

	return FoundReg;
}






//****************************************************
//RN8209发送数据
//****************************************************
static void RN8209_Send(u8 Data[], u8 Len)
{
	memcpy(Uart3.Tx.pBuf, Data, Len);
	Uart3.Tx.Len = Len;

	HAL_UART_Transmit_DMA(&huart3, Uart3.Tx.pBuf, Uart3.Tx.Len);
}





static void RN8209_Swap_Data(u8 S_Data[], u8 Len, u8 O_Data[])
{
	u8 i = 0 ;

	for(i=0; i<Len; i++)
	{
		O_Data[Len-1-i] = S_Data[i];
	}
}

//****************************************************
//RN8209写寄存器
//****************************************************
EN_Global_Status RN8209_Write_Reg(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len)
{
	u8 tmpData1[RN8209_BUF_SIZE] = {0};
	u8 tmpData2[RN8209_BUF_SIZE] = {0};
	u8 i = 0 ;

	if(If_Found_RN8209_Reg(RegAddr) == false)
	{
		return Status_Error;
	}

	if((Len == 0) || (Len > 4))
	{
		return Status_Error;
	}

	//大小端格式转换，先发送高字节
	RN8209_Swap_Data(Data, Len, tmpData1);

	tmpData2[0] = RegAddr | 0x80;

	for(i=0; i<Len; i++)
	{
		tmpData2[i+1] = tmpData1[i];
	}

	Len += 1;

	tmpData2[Len] = (~(Check_Sum(tmpData2, Len)));

	Len += 1;

	RN8209_Send(tmpData2, Len);

	HAL_Delay(TIME_25MS);

	return Status_Success;
}





static bool RN8209_Check_Reg_Pack(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len)
{
	u8 tmpData[RN8209_BUF_SIZE];
	u8 i = 0 ;
	u8 TempCSM = 0;

	tmpData[0] = RegAddr;

	for(i=0; i<(Len-1); i++)
	{
		tmpData[i+1] = Data[i];
	}

	TempCSM = (~(Check_Sum(tmpData, Len)));

	if(Data[Len-1] == TempCSM)
	{
		return true;
	}

	return false;
}

static EN_Global_Status rn8209_ParseCmd(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len)
{
	EN_Global_Status Status = Status_Success;
	u32 TempReg = 0;
	u8 Idx = 0;

	for(Idx=0; Idx<Len; Idx++)
	{
		TempReg = (TempReg | (Data[Idx] << ((Len - 1 - Idx) * 0x08)));
	}

	switch(RegAddr)
	{
		case ADDR_URMS:
			if(TempReg & 0x00800000)
			{
				RN8209_RegRcv.Data.URMS = 0;
			}else{
				RN8209_RegRcv.Data.URMS = TempReg;
			}
			break;
		case ADDR_IARMS:
			if(TempReg & 0x00800000)
			{
				RN8209_RegRcv.Data.IARMS = 0;
			}else{
				RN8209_RegRcv.Data.IARMS = TempReg;
			}
			break;
		case ADDR_PowerPA:
			RN8209_RegRcv.Data.PowerPA = TempReg;
			//RN8209_RegRcv.Data.PowerPA = 0xffffffff;
			break;
		case ADDR_EnergyP:
			RN8209_RegRcv.Data.EnergyP = TempReg;
			break;
		case ADDR_IBRMS:
			if(TempReg & 0x00800000)
			{
				RN8209_RegRcv.Data.IBRMS = 0;
			}else{
				RN8209_RegRcv.Data.IBRMS = TempReg;
			}
			break;
		case ADDR_PowerPB:
			RN8209_RegRcv.Data.PowerPB = TempReg;
			break;
		case ADDR_EnergyD:
			RN8209_RegRcv.Data.EnergyD = TempReg;
			break;
		case ADDR_EMUStatus:
			RN8209_RegRcv.Data.EMUStatus = TempReg;

			//校验和有效
			if((RN8209_RegRcv.Data.EMUStatus & 0x10000) == 0)
			{
				if(RN8209_CheckSum != (RN8209_RegRcv.Data.EMUStatus & 0x0000ffff))
				{
					return Status_Error;
				}
			}
			break;
		case ADDR_PFCnt:
			RN8209_RegRcv.Data.PFCnt = TempReg;
			break;
		case ADDR_DFcnt:
			RN8209_RegRcv.Data.DFcnt = TempReg;
			break;
		default:
			break;
	}

	return Status;
}

//****************************************************
//RN8209读寄存器
//****************************************************
EN_Global_Status RN8209_Read_Reg(EN_RN8209_REG_ADDR RegAddr)
{
	EN_Global_Status Status = Status_Success;
	u8 tmpData[RN8209_BUF_SIZE];
	u8 Len = 0;
	u32 WaitTick = 0;

	if(If_Found_RN8209_Reg(RegAddr) == false)
	{
		return Status_Error;
	}

	//发送要读取的寄存器
	RN8209_Send((u8*)&RegAddr, 1);

	Reset_Tick(&WaitTick);

	while(1)
	{
		if(Uart3.Rx.Len > 0)
		{
			memcpy(tmpData, Uart3.Rx.pBuf, Uart3.Rx.Len);
			Len = Uart3.Rx.Len;
		
			Uart3.Rx.Len = 0;
		
			if(RN8209_Check_Reg_Pack(RegAddr, tmpData, Len))
			{
				Status = rn8209_ParseCmd(RegAddr, tmpData, (Len-1));
		
				return Status;
			}else{
				return Status_Error;
			}
		}
		else if(Tick_Timeout(&WaitTick, TIME_25MS))
		{
			return Status_Timeout;
		}
	}
}




