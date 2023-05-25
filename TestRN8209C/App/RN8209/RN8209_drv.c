#include "RN8209_drv.h"
#include "RN8209_reg.h"
#include "usart.h"
#include "app_tool.h"





#define RN8209_BUF_SIZE UART3_BUF_SIZE


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





static EN_Global_Status RN8209_Check_Reg_Pack(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len)
{
	EN_Global_Status Status = Status_Error;
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
		Status = Status_Success;
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

			Status = RN8209_Check_Reg_Pack(RegAddr, tmpData, Len);
			if(Status == Status_Success)
			{
				Status = RN8209_Parse_Cmd(RegAddr, tmpData, (Len-1));
			}

			break;
		}
		else if(Tick_Timeout(&WaitTick, TIME_25MS))
		{
			Status = Status_Timeout;
			break;
		}
	}

	return Status;
}




