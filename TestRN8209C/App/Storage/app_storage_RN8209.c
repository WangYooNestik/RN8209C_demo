#include "app_storage_RN8209.h"
#include "app_storage.h"
#include "app_test.h"
#include "RN8209_init.h"





ST_STORAGE_RN8209 Storage_RN8209;


void Init_Storage_RN8209_Variables(void)
{
	memset(&Storage_RN8209, 0, sizeof(Storage_RN8209));
}

static void Init_Storage_RN8209_Data(void)
{
	Init_Storage_RN8209_Variables();

//	Storage_Set_NeedSave_Flag(STORAGE_RN8209);
}

void Init_Storage_RN8209_Info(ST_STORAGE_INFO *StorageInfo)
{
	StorageInfo->Type     = STORAGE_RN8209;
	StorageInfo->InitData = Init_Storage_RN8209_Data;
	StorageInfo->RamAddr  = (ST_STORAGE_DATA_P *)&Storage_RN8209;
	StorageInfo->Len      = sizeof(Storage_RN8209);
}

static void Check_Storage_RN8209_Value(void)
{
	bool NeedSave = false;

	if(NeedSave)
	{
		Storage_Set_NeedSave_Flag(STORAGE_RN8209);
	}
}

void Load_Storage_RN8209(void)
{
	ST_STORAGE_INFO StorageInfo;
	
	Init_Storage_RN8209_Info(&StorageInfo);
	Storage_Check_Main_Area(StorageInfo);			//读取主存储区和备份存储区的数据，检查存储的数据是否正确
	
	Check_Storage_RN8209_Value();					//对一些可能造成参数越界，系统死机的变量，检查参数的有效性
}



