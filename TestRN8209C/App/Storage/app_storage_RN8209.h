#ifndef _APP_STORAGE_RN8209_H_
#define _APP_STORAGE_RN8209_H_

#include "app_storage_drv.h"
#include "RN8209_drv.h"





typedef struct{
	ST_STORAGE_CHECK StorageCheck;
	ST_RN8209_CTL_REG CtlReg;
	float U_Gain;
	float IA_Gain;
	float IB_Gain;

	ST_RN8209_DATA_REG DataReg;
	u64 EA_Count;
	u64 EB_Count;
}ST_STORAGE_RN8209;

extern ST_STORAGE_RN8209 Storage_RN8209;


void Init_Storage_RN8209_Variables(void);
void Init_Storage_RN8209_Info(ST_STORAGE_INFO *StorageInfo);
void Load_Storage_RN8209(void);

#endif


