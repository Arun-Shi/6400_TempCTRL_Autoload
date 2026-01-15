#ifndef __Fans_H__
#define __Fans_H__

#include "./Base_Head_File.h"

#define __Fans_Chx_Num 8

typedef enum{
	Fans_Stop=0,				//停止风扇
	Fans_Run_Max,				//循环风扇满转速
	Fans_Run_Dot8,				//循环风扇80%转速
	Fans_Run_Dot6,
	Fans_Run_Dot3,
	Fans_Run_Dissipation,		//散热状态
}__Fans_Run_State;

typedef enum{
	Cross_Front=0,
	Cross_Rear,
	A_Side_L_D,
	A_Side_M_D,
	A_Side_R_D,
	A_Side_L_U,
	A_Side_M_U,
	A_Side_R_U,
	B_Side_L_D,
	B_Side_M_D,
	B_Side_R_D,
	B_Side_L_U,
	B_Side_M_U,
	B_Side_R_U,
}__Fans_Chx;

typedef struct __Fan_Struct{
	float Scal_AB;
	float Scal_LMR;
	int Out_Duty;
	u8 Chx;
}Fan_Struct;

extern Fan_Struct  Fans_ALL[14];

void Fans_ALL_Init(void);
u8 Fans_Set_Chx_OutP(__Fans_Chx Chx, int Out_P);
u8 Fans_Set_Switch_Status(__Fans_Run_State Status);
void Fans_ALL_Set_Duty(void);


#endif
