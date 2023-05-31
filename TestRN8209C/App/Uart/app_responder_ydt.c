#include "app_responder_ydt.h"
#include "app_uart_rcv.h"
#include "app_test.h"
#include "app_tool.h"
#include "RN8209_main.h"
#include "RN8209_calibrate.h"
#include "RN8209_read.h"
#include "app_storage.h"
#include "app_storage_RN8209.h"





static EN_CID2_RTN Parse_Responder_Cmd(void)
{
	EN_CID2_RTN CheckRTN = CID2_RTN_OK;
	u16 Idx1 = 0;
	u16 Idx2 = 0;

	Idx1 = YDT_IDX_DATA;
	Idx2 = 0;

	switch(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID2])
	{
		case CID2_RCV_CALIBRATE_STATUS:
			if(Get_RN8209_Main_State() == RN8209_MAIN_RUN)
			{
				UartRcv.Data.Temp2.pBuf[Idx2++] = 0;
			}else{
				UartRcv.Data.Temp2.pBuf[Idx2++] = 1;
			}
			break;
		case CID2_RCV_CALIBRATE_ZERO:
			RN8209_Set_Calibrate_Func(RN8209_CALIBRATE_FUNC_ZERO);
			break;
		case CID2_RCV_CALIBRATE_U:
			RN8209_Set_Calibrate_Func(RN8209_CALIBRATE_FUNC_U_GAIN);
			break;
		case CID2_RCV_CALIBRATE_I_AND_P:
			RN8209_Set_Calibrate_Func(RN8209_CALIBRATE_FUNC_I_AND_P_GAIN);
			break;
		case CID2_RCV_CALIBRATE_P_OFFSET:
			RN8209_Set_Calibrate_Func(RN8209_CALIBRATE_FUNC_P_GAIN_OFFSET);
			break;
		case CID2_RCV_GET_ANALOG:
			Float_To_DataBuf(RN8209_Analog.Voltage,   &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Current_A, &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Power_A,   &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Energy_A,  &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Current_B, &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Power_B,   &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			Float_To_DataBuf(RN8209_Analog.Energy_B,  &UartRcv.Data.Temp2.pBuf[Idx2], &Idx2, 4, 3);
			break;
		case CID2_RCV_CLEAR_ENERGY:
			Storage_RN8209.EA_Count = 0;
			Storage_RN8209.EB_Count = 0;
			Storage_RN8209.DataReg.PFCnt = 0;
			Storage_RN8209.DataReg.DFcnt = 0;
			Storage_Set_NeedSave_Flag(STORAGE_RN8209);
			break;
		default:
			break;
	}

	UartRcv.Data.Temp2.Len = Idx2;

	return CheckRTN;
}

//优先检查地址，避免同串口下并联其它设备
EN_CID2_RTN Parse_Responder_ydt(void)
{
	EN_CID2_RTN CheckRTN = CID2_RTN_OK;

	UartRcv.Data.Temp2.Len = 0;

	if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID2] == CID2_RCV_GET_ADDR \
	|| UartRcv.Data.Temp1.pBuf[YDT_IDX_CID2] == CID2_RCV_SET_ADDR)
	{
		if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID1] == CID1_RTN)
		{
			if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID2] == CID2_RCV_SET_ADDR)			//设置设备地址
			{
				if(UartRcv.Data.Temp1.pBuf[YDT_IDX_VER] == DEV_VER)
				{
					if(UartRcv.Data.Temp1.pBuf[YDT_IDX_ADDR] > 0 && UartRcv.Data.Temp1.pBuf[YDT_IDX_ADDR] < 0xff)
					{
						if(DEV_ADDR != UartRcv.Data.Temp1.pBuf[YDT_IDX_ADDR])
						{
							//DEV_ADDR = UartRcv.Data.Temp1.pBuf[YDT_IDX_ADDR];

							//Storage_Set_NeedSave_Flag(STORAGE_SYS_CFG);
						}
					}else{
						CheckRTN = CID2_RTN_DATA_ERROR;
					}
				}else{
					CheckRTN = CID2_RTN_VER_ERROR;
				}
			}
		}else{
			CheckRTN = CID2_RTN_CID1_ERROR;
		}
	}else{
		if(UartRcv.Data.Temp1.pBuf[YDT_IDX_ADDR] == DEV_ADDR)
		{
			if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID2] == CID2_RCV_GET_VERSION)		//获取版本号
			{
				if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID1] != CID1_RTN)
				{
					CheckRTN = CID2_RTN_CID1_ERROR;
				}
			}else{
				if(UartRcv.Data.Temp1.pBuf[YDT_IDX_VER] == DEV_VER)					//协议版本
				{
					if(UartRcv.Data.Temp1.pBuf[YDT_IDX_CID1] == CID1_RTN)
					{
						CheckRTN = Parse_Responder_Cmd();
					}else{
						CheckRTN = CID2_RTN_CID1_ERROR;
					}
				}else{
					CheckRTN = CID2_RTN_VER_ERROR;
				}
			}
		}else{
			CheckRTN = CID2_RTN_ADDR_ERROR;
		}
	}

	Swap_Buf(&UartRcv.Data.Temp1, &UartRcv.Data.Temp2);

	return CheckRTN;
}



