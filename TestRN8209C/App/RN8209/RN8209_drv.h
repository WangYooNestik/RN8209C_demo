#ifndef _RN8209_DRV_H_
#define _RN8209_DRV_H_

#include "app_global.h"
#include "usart.h"





extern u16 RN8209_CheckSum;

#define RN8209_BUF_SIZE UART3_BUF_SIZE

typedef enum{
	Status_Success = 0,
	Status_Error,
	Status_Timeout,
	Status_ProtocolError,
	Status_Enable,
	Status_Wait,
}EN_RN8209_Read_Write_Status;

//---- RN8209 地址定义----//
typedef enum{
	ADDR_SYSCON = 0x00,		//系统控制寄存器
	ADDR_EMUCON = 0x01,		//计量控制寄存器
	ADDR_HFConst = 0x02,	//脉冲频率寄存器
	ADDR_PStart = 0x03,		//有功起动功率设置
	ADDR_DStart = 0x04,		//自定义电能起动功率设置
	ADDR_GPQA = 0x05,
	ADDR_GPQB = 0x06,
	ADDR_PhsA = 0x07,
	ADDR_PhsB = 0x08,
	ADDR_QPHSCAL = 0x09,
	ADDR_APOSA = 0x0a,		//通道A有功功率Offset校正寄存器
	ADDR_APOSB = 0x0b,		//通道B有功功率Offset校正寄存器
	ADDR_RPOSA = 0x0c,
	ADDR_RPOSB = 0x0d,
	ADDR_IARMSOS = 0x0e,
	ADDR_IBRMSOS = 0x0f,
	ADDR_IBGain = 0x10,		//电流通道B增益设置
	ADDR_D2FPL = 0x11,		//自定义功率寄存器低
	ADDR_D2FPH = 0x12,		//自定义功率寄存器高
	ADDR_DCIAH = 0x13,
	ADDR_DCIBH = 0x14,
	ADDR_DCUH = 0x15,
	ADDR_DCL = 0x16,
	ADDR_EMUCON2 = 0x17,	//计量控制寄存器2

	ADDR_PFCnt = 0x20,
	ADDR_DFcnt = 0x21,
	ADDR_IARMS = 0x22,		//通道A电流有效值
	ADDR_IBRMS = 0x23,		//通道B电流有效值
	ADDR_URMS = 0x24,		//电压有效值
	ADDR_UFreq = 0x25,		//频率
	ADDR_PowerPA = 0x26,	//有功功率A
	ADDR_PowerPB = 0x27,	//有功功率B
	ADDR_PowerQ = 0x28,		//无功功率
	ADDR_EnergyP = 0x29,	//有功能量，默认通道A
	ADDR_EnergyP2 = 0x2a,
	ADDR_EnergyD = 0x2b,	//有功能量，配置成通道B
	ADDR_EnergyD2 = 0x2c,
	ADDR_EMUStatus = 0x2d,	//计量状态寄存器
	ADDR_SPL_IA = 0x30,		//IA通道电流采样值
	ADDR_SPL_IB = 0x31,		//IB通道电流采样值
	ADDR_SPL_U = 0x32,		//U通道电流采样值
	ADDR_UFreq2 = 0x35,
	ADDR_IE = 0x40,
	ADDR_IF = 0x41,
	ADDR_RIF = 0x42,
	ADDR_SysStatus = 0x43,
	ADDR_RData = 0x44,
	ADDR_WData = 0x45,
	ADDR_DeviceID = 0x7f,

	ADDR_SysCtl = 0xea,		//特殊命令（抽象为一个寄存器）
}EN_RN8209_REG_ADDR;

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


EN_RN8209_Read_Write_Status RN8209_Write_Reg_Swap(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len);
EN_RN8209_Read_Write_Status RN8209_Read_Reg_Swap(EN_RN8209_REG_ADDR RegAddr);

#endif

