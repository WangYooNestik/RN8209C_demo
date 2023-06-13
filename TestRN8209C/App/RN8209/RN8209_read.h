#ifndef _RN8209_READ_H_
#define _RN8209_READ_H_

#include "RN8209_reg.h"





extern ST_RN8209_DATA_REG RN8209_AverageData;

typedef struct{
	float U;
	float IA;
	float PA;
	float PA1;
	double EA;
	float IB;
	float PB;
	float PB1;
	double EB;
}ST_RN8209_ANALOG;

extern ST_RN8209_ANALOG RN8209_Analog;


void RN8209_Read_Handler(void);

#endif

