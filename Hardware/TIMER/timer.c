#include "./TIMER/timer.h"
//系统基础时钟加溢出中断初始化，Feq为中断频率
void Sys_Base_TIM_Init(u16 Feq)			//基础时钟-Tim2
{
	u32 ARR_T;
	u16 PSC_T;
	
	for(PSC_T=1;;)
	{
		ARR_T = Sys_Tim_Feq/ (Feq*PSC_T);
		if(ARR_T>U16_MAX || Sys_Tim_Feq% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR_T-1,
		.TIM_Prescaler =PSC_T-1,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
	};
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure={
		.NVIC_IRQChannel = TIM5_IRQn,  			
		.NVIC_IRQChannelPreemptionPriority = 3,  	
		.NVIC_IRQChannelSubPriority = 3,			
		.NVIC_IRQChannelCmd = ENABLE,				
	};
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	
	TIM_Cmd(TIM5, ENABLE);
}

