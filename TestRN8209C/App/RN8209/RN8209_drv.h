#ifndef _RN8209_DRV_H_
#define _RN8209_DRV_H_

#include "RN8209_reg_addr.h"





EN_Global_Status RN8209_Write_Reg(EN_RN8209_REG_ADDR RegAddr, u8 Data[], u8 Len);
EN_Global_Status RN8209_Read_Reg(EN_RN8209_REG_ADDR RegAddr);

#endif

