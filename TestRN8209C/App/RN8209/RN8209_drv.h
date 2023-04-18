#ifndef _RN8209_DRV_H_
#define _RN8209_DRV_H_

#include "app_global.h"
#include "usart.h"
#include "RN8209_reg_addr.h"





extern u16 RN8209_CheckSum;

#define RN8209_BUF_SIZE UART3_BUF_SIZE

//---- RN8209 寄存器缓存----//

#define u24 u32
#define s24 s32

typedef struct{
	u16 SYSCON;
	u16 EMUCON;
	u16 HFConst;
	u16 PStart;
	u16 DStart;
	s16 GPQA;
	s16 GPQB;
	s8 PhsA;
	s8 PhsB;
	s16 QPHSCAL;
	s16 APOSA;
	s16 APOSB;
	s16 RPOSA;
	s16 RPOSB;
	s16 IARMSOS;
	s16 IBRMSOS;
	s16 IBGain;
	u16 D2FPL;
	s16 D2FPH;
	u16 DCIAH;
	u16 DCIBH;
	u16 DCUH;
	u16 DCL;
	u16 EMUCON2;
}ST_RN8209_CTL_REG;

extern ST_RN8209_CTL_REG RN8209_CtlReg;

typedef struct{
	u16 PFCnt;
	u16 DFcnt;
	s24 IARMS;
	s24 IBRMS;
	s24 URMS;
	u16 UFreq;
	s32 PowerPA;
	s32 PowerPB;
	s32 PowerQ;
	u24 EnergyP;
	u24 EnergyP2;
	u24 EnergyD;
	u24 EnergyD2;
	u24 EMUStatus;
	u24 SPL_IA;
	u24 SPL_IB;
	u24 SPL_U;
	u24 UFreq2;
}ST_RN8209_DATA_REG;

extern ST_RN8209_DATA_REG RN8209_DataReg;

typedef struct{
	u8 IE;
	u8 IF;
	u8 RIF;
}ST_RN8209_INT_REG;

typedef struct{
	u8 SysStatus;
	u32 RData;
	u16 WData;
	u24 DeviceID;
}ST_RN8209_STA_REG;

typedef struct{
	ST_RN8209_CTL_REG Control;		//控制寄存器
	ST_RN8209_DATA_REG Data;		//数据和状态
	ST_RN8209_INT_REG interrupt;	//中断
	ST_RN8209_STA_REG Status;		//系统状态
}ST_RN8209_REG;

extern ST_RN8209_REG RN8209_RegRcv;


typedef enum{
	UNLOCK = 0xe5,
	LOCK = 0xdc,
	SLC_CH_A = 0x5a,
	SLC_CH_B = 0xa5,
	RST = 0xfa,
}EN_RN8209_SPC_REG;

typedef struct{
	u8 Unlock;
	u8 Lock;
	u8 SelectChA;
	u8 SelectChB;
	u8 Reset;
}ST_RN8209_SYSCTL;

extern const ST_RN8209_SYSCTL RN8209_SysCtl;


EN_Global_Status RN8209_Write_Reg_Swap(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len);
EN_Global_Status RN8209_Read_Reg_Swap(EN_RN8209_REG_ADDR RegAddr);

#endif

