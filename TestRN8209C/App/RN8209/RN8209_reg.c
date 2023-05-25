#include "RN8209_reg.h"





//用于计算校验
u16 RN8209_CheckSum = 0;

//用来直接跟真正的寄存器值同步，不管是写入还是读出RN8209，都是用的这块内存上的值
ST_RN8209_REG RN8209_Reg;


bool If_Found_RN8209_Reg(EN_RN8209_REG_ADDR RegAddr)
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

EN_Global_Status RN8209_Parse_Cmd(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len)
{
	EN_Global_Status Status = Status_Success;
	u32 TempReg = 0;
	u8 Idx = 0;

	if(If_Found_RN8209_Reg(RegAddr) == false)
	{
		return Status_Error;
	}

	if((Len == 0) || (Len > 4))
	{
		return Status_Error;
	}

	for(Idx=0; Idx<Len; Idx++)
	{
		TempReg = (TempReg | (Data[Idx] << ((Len - 1 - Idx) * 0x08)));
	}

	switch(RegAddr)
	{
		case ADDR_URMS:
			if(TempReg & 0x00800000)
			{
				RN8209_Reg.Data.URMS = 0;
			}else{
				RN8209_Reg.Data.URMS = TempReg;
			}
			break;
		case ADDR_IARMS:
			if(TempReg & 0x00800000)
			{
				RN8209_Reg.Data.IARMS = 0;
			}else{
				RN8209_Reg.Data.IARMS = TempReg;
			}
			break;
		case ADDR_PowerPA:
			RN8209_Reg.Data.PowerPA = TempReg;
			//RN8209_Reg.Data.PowerPA = 0xffffffff;
			break;
		case ADDR_EnergyP:
			RN8209_Reg.Data.EnergyP = TempReg;
			break;
		case ADDR_IBRMS:
			if(TempReg & 0x00800000)
			{
				RN8209_Reg.Data.IBRMS = 0;
			}else{
				RN8209_Reg.Data.IBRMS = TempReg;
			}
			break;
		case ADDR_PowerPB:
			RN8209_Reg.Data.PowerPB = TempReg;
			break;
		case ADDR_EnergyD:
			RN8209_Reg.Data.EnergyD = TempReg;
			break;
		case ADDR_EMUStatus:
			RN8209_Reg.Data.EMUStatus = TempReg;

			//校验和有效
			if((RN8209_Reg.Data.EMUStatus & 0x10000) == 0)
			{
				if(RN8209_CheckSum != (RN8209_Reg.Data.EMUStatus & 0x0000ffff))
				{
					Status = Status_Error;
				}
			}
			break;
		case ADDR_PFCnt:
			RN8209_Reg.Data.PFCnt = TempReg;
			break;
		case ADDR_DFcnt:
			RN8209_Reg.Data.DFcnt = TempReg;
			break;
		default:
			break;
	}

	return Status;
}



