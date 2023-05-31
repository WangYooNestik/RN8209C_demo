#ifndef _RN8209_MAIN_H_
#define _RN8209_MAIN_H_





typedef enum{
	RN8209_MAIN_INIT = 0,
	RN8209_MAIN_CALIBRATE,		//校准基准的时候，必须要将输入接地
	RN8209_MAIN_RUN,
}EN_RN8209_MAIN_STATE;

#define IF_RN8209_MAIN_STATE_OK(State) if(State > RN8209_MAIN_RUN) return


EN_RN8209_MAIN_STATE Get_RN8209_Main_State(void);
void Set_RN8209_Main_State(EN_RN8209_MAIN_STATE State);
void RN8209_Handler(void);

#endif

