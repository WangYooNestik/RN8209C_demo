#ifndef _RN8209_READ_H_
#define _RN8209_READ_H_

#include "RN8209_reg.h"





#define WAIT_DATA_REFRESH_TIME TIME_10S

extern ST_RN8209_DATA_REG RN8209_AverageData;

typedef struct{
	float Voltage;
	float Current_A;
	float Power_A;
	float Power_A_1;
	double Energy_A;
	float Current_B;
	float Power_B;
	float Power_B_1;
	double Energy_B;
}ST_RN8209_ANALOG;

extern ST_RN8209_ANALOG RN8209_Analog;


void RN8209_Read(void);

#endif

