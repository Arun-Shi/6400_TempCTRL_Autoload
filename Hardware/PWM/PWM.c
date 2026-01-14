#include "./PWM/PWM.h"
void PWM3_Operate(u8 Ope)
{
	if(Ope==PWM_CLOSE)
		TIM_Cmd(TIM3, DISABLE);
	else if(Ope==PWM_OPEN)
		TIM_Cmd(TIM3, ENABLE);
}

void PWM4_Operate(u8 Ope)
{
	if(Ope==PWM_CLOSE)
		TIM_Cmd(TIM4, DISABLE);
	else if(Ope==PWM_OPEN)
		TIM_Cmd(TIM4, ENABLE);
}

void PWM3_Chx_Set(u8 Chx, u32 Feq, int Duty)
{
	u32 ARR_T;
	u16 PSC_T;
	u16 CCR_T;
	for(PSC_T=1;;)
	{
		ARR_T = (unsigned long)PWM3_F/(Feq*PSC_T);
		if(ARR_T>U16_MAX || PWM3_F% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	if(Duty==0)
		CCR_T=1;
	else
		CCR_T = ARR_T*Duty/100;
	
	TIM3_PWM_Init(Chx, ARR_T-1, PSC_T-1, CCR_T-1);
}
void PWM4_Chx_Set(u8 Chx, u32 Feq, int Duty)
{
	u32 ARR_T;
	u16 PSC_T;
	u16 CCR_T;
	
	for(PSC_T=1;;)
	{
		ARR_T = (unsigned long)PWM4_F/(Feq*PSC_T);
		if(ARR_T>U16_MAX || PWM4_F% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	if(Duty==0)
		CCR_T=1;
	else
		CCR_T = ARR_T*Duty/100;
	TIM4_PWM_Init(Chx, ARR_T-1, PSC_T-1, CCR_T-1);
}
void PWM2_Chx_Set(u8 Chx, u32 Feq, int Duty)
{
	u32 ARR_T;
	u16 PSC_T;
	u16 CCR_T;
	
	for(PSC_T=1;;)
	{
		ARR_T = (unsigned long)PWM4_F/(Feq*PSC_T);
		if(ARR_T>U16_MAX || PWM4_F% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	if(Duty==0)
		CCR_T=1;
	else
		CCR_T = ARR_T*Duty/100;
	TIM2_PWM_Init(Chx, ARR_T-1, PSC_T-1, CCR_T-1);
}
void PWM8_Chx_Set(u8 Chx, u32 Feq, int Duty)
{
	u32 ARR_T;
	u16 PSC_T;
	u16 CCR_T;
	
	for(PSC_T=1;;)
	{
		ARR_T = (unsigned long)PWM4_F/(Feq*PSC_T);
		if(ARR_T>U16_MAX || PWM4_F% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	if(Duty==0)
		CCR_T=1;
	else
		CCR_T = ARR_T*Duty/100;
	TIM8_PWM_Init(Chx, ARR_T-1, PSC_T-1, CCR_T-1);
}

