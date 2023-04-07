#ifndef _RN8209_CALIBRATE_GAIN_H_
#define _RN8209_CALIBRATE_GAIN_H_

#include "app_global.h"





typedef enum{
	RN8209_CLB_GAIN_VOLTAGE = 0,
	RN8209_CLB_GAIN_CH_A,
	RN8209_CLB_GAIN_CH_B,
	RN8209_CLB_GAIN_MAX
}EN_RN8209_CLB_GAIN_TYPE;


void RN8209_Calibrate_Gain_Set_Type(EN_RN8209_CLB_GAIN_TYPE Ch);
void RN8209_Init_Calibrate_Gain_State(void);
bool RN8209_Calibrate_Gain_Handler(void);

#endif

