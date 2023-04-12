#ifndef _RN8209_FUNC_H_
#define _RN8209_FUNC_H_

#include "app_global.h"





typedef enum{
	CH_I_GAIN_1 = 1,		//1倍增益，不增益
	CH_I_GAIN_2,			//2倍增益
	CH_I_GAIN_8 = 8,		//8倍增益
}EN_CH_I_GAIN;				//电流通道增益

#define CH_I_GAIN CH_I_GAIN_8





#define RN8209_Un 48.0f		//额定电压
#define RN8209_Ib 50.0f		//额定电流
#define RN8209_EC 3200		//电表常数

#define U_S_Gain 331		//电压采样电阻比例
#define I_S_Gain_1 4000		//电流采样电阻比例，采样电阻250μΩ，（I_S_Gain_1 = 1 / 250μΩ）
#define I_S_Gain_2 2000		//电流采样电阻比例，采样电阻500μΩ，（I_S_Gain_2 = 1 / 500μΩ）
#define I_S_Gain I_S_Gain_2	//电流采样电阻比例

//功率转换系数Kp
//#define KP_VALUE 0.00061683845399927936102214612458882f
//#define KP_VALUE 0.00061653554439544677734375f
#define KP_VALUE (2 * U_S_Gain * I_S_Gain / (pow(2, 32) * CH_I_GAIN))


void RN8209_Init_Variables(void);
void RN8209_Reset(void);
u16 RN8209_Init_Ctl_Reg(void);
u16 RN8209_Init_Zero_Reg(void);
u16 RN8209_Init_Zero_Reg_1(void);
u16 RN8209_Init_Gain_Reg(void);
u16 RN8209_Init_Offset_Reg(void);
void RN8209_Init(void);

#endif

