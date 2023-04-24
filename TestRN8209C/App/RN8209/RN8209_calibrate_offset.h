#ifndef _RN8209_CALIBRATE_OFFSET_H_
#define _RN8209_CALIBRATE_OFFSET_H_

#include "app_global.h"





typedef enum{
	RN8209_CLB_OFFSET_CH_A = 0,
	RN8209_CLB_OFFSET_CH_B,
	RN8209_CLB_OFFSET_MAX
}EN_RN8209_CLB_OFFSET_TYPE;


void RN8209_Calibrate_Offset_Set_Type(EN_RN8209_CLB_OFFSET_TYPE Ch);
bool RN8209_Calibrate_Offset_Handler(void);

#endif

