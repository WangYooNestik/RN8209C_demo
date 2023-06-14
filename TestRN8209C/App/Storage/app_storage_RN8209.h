#ifndef _APP_STORAGE_RN8209_H_
#define _APP_STORAGE_RN8209_H_

#include "app_storage_drv.h"
#include "RN8209_reg.h"





typedef struct{
	ST_STORAGE_CHECK StorageCheck;
	ST_RN8209_CTRL_REG CtrlReg;
	ST_RN8209_DATA_REG DataReg;
	float U_Gain;
	float IA_Gain;
	float IB_Gain;
	u64 EA_Count;			//总电量
	u64 EB_Count;
	ST_MY_FLOAT EA;
	ST_MY_FLOAT EB;
}ST_STORAGE_RN8209;

extern ST_STORAGE_RN8209 Storage_RN8209;


void Init_Storage_RN8209_Variables(void);
void Init_Storage_RN8209_Info(ST_STORAGE_INFO *StorageInfo);
void Load_Storage_RN8209(void);

#endif


