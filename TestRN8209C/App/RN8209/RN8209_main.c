#include "RN8209_main.h"
#include "RN8209_init.h"
#include "RN8209_calibrate.h"
#include "RN8209_read.h"





typedef struct{
	EN_RN8209_MAIN_STATE State;
	
}ST_RN8209;

ST_RN8209 RN8209;


EN_RN8209_MAIN_STATE Get_RN8209_Main_State(void)
{
	return RN8209.State;
}

void Set_RN8209_Main_State(EN_RN8209_MAIN_STATE State)
{
	IF_RN8209_MAIN_STATE_OK(State);

	RN8209.State = State;
}

void RN8209_Handler(void)
{
	switch(RN8209.State)
	{
		case RN8209_MAIN_INIT:
			RN8209_Init(RN8209_INIT_ALL);
			RN8209.State = RN8209_MAIN_RUN;
			break;
		case RN8209_MAIN_CALIBRATE:
			//校准基准的时候，必须要将输入接地
			if(RN8209_Calibrate_Handler() == Status_Success)
			{
				RN8209.State = RN8209_MAIN_INIT;
			}
			break;
		default:
			break;
	}

	RN8209_Read_Handler();
}

