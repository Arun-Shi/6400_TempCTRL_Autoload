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

void TIM3_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR )
{ 
#ifndef __TIM_ALL_INIT
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR,
		.TIM_Prescaler = PSC,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
	};
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure={
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OCPolarity = TIM_OCPolarity_Low,
	};

	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM3_PWM_Init(1, ARR, PSC, CCR);
			TIM3_PWM_Init(2, ARR, PSC, CCR);
			TIM3_PWM_Init(3, ARR, PSC, CCR);
			TIM3_PWM_Init(4, ARR, PSC, CCR);
		break;
		case 1:
			SmpIO_Init(A6, AF_PP);
			TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		break;

		case 2:
			SmpIO_Init(A7, AF_PP);
			TIM_OC2Init(TIM3, &TIM_OCInitStructure);
		break;
		
		case 3:
			SmpIO_Init(B0, AF_PP);
			TIM_OC3Init(TIM3, &TIM_OCInitStructure);
		break;
		
		case 4:
			SmpIO_Init(B1, AF_PP);
			TIM_OC4Init(TIM3, &TIM_OCInitStructure);
		break;
			TIM_Cmd(TIM3, ENABLE);
	}
#endif	
	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM3_PWM_Init(1, ARR, PSC, CCR);
			TIM3_PWM_Init(2, ARR, PSC, CCR);
			TIM3_PWM_Init(3, ARR, PSC, CCR);
			TIM3_PWM_Init(4, ARR, PSC, CCR);
		break;
		case 1:
			TIM_SetCompare1(TIM3, CCR);
		break;
		case 2:
			TIM_SetCompare2(TIM3, CCR);
		break;
		case 3:
			TIM_SetCompare3(TIM3, CCR);
		break;
		case 4:
			TIM_SetCompare4(TIM3, CCR);
		break;
	}
}

//对任意通道的初始化都会修改同组通道的频率
void TIM4_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR)
{
#ifndef __TIM_ALL_INIT	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR,
		.TIM_Prescaler = PSC,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
	};
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure={
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OCPolarity = TIM_OCPolarity_Low,
	};

	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM4_PWM_Init(1, ARR, PSC, CCR);
			TIM4_PWM_Init(2, ARR, PSC, CCR);
			TIM4_PWM_Init(3, ARR, PSC, CCR);
			TIM4_PWM_Init(4, ARR, PSC, CCR);
		break;
		
		case 1:
			SmpIO_Init(D12, AF_PP);
			TIM_OC1Init(TIM4, &TIM_OCInitStructure);
			TIM_SetCompare1(TIM4, CCR);
		break;
		
		case 2:
			SmpIO_Init(D13, AF_PP);
			TIM_OC2Init(TIM4, &TIM_OCInitStructure);
			TIM_SetCompare2(TIM4, CCR);
		break;
		
		case 3:
			SmpIO_Init(D14, AF_PP);
			TIM_OC3Init(TIM4, &TIM_OCInitStructure);
			TIM_SetCompare3(TIM4, CCR);
		break;
		
		case 4:
			SmpIO_Init(D15, AF_PP);
			TIM_OC4Init(TIM4, &TIM_OCInitStructure);
			TIM_SetCompare4(TIM4, CCR);
		break;
	}
	TIM_Cmd(TIM4, ENABLE);
#endif
	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM4_PWM_Init(1, ARR, PSC, CCR);
			TIM4_PWM_Init(2, ARR, PSC, CCR);
			TIM4_PWM_Init(3, ARR, PSC, CCR);
			TIM4_PWM_Init(4, ARR, PSC, CCR);
		break;
		
		case 1:
			TIM_SetCompare1(TIM4, CCR);
		break;
		
		case 2:
			TIM_SetCompare2(TIM4, CCR);
		break;
		
		case 3:
			TIM_SetCompare3(TIM4, CCR);
		break;
		
		case 4:
			TIM_SetCompare4(TIM4, CCR);
		break;
	}
}
void TIM2_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR)
{
#ifndef __TIM_ALL_INIT
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR,
		.TIM_Prescaler = PSC,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
	};
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure={
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OCPolarity = TIM_OCPolarity_Low,
	};

	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM2_PWM_Init(1, ARR, PSC, CCR);
			TIM2_PWM_Init(2, ARR, PSC, CCR);
		break;
		
		case 1:
			SmpIO_Init(A15, AF_PP);
			TIM_OC1Init(TIM2, &TIM_OCInitStructure);
			TIM_SetCompare1(TIM2, CCR);
		break;
		
		case 2:
			SmpIO_Init(B3, AF_PP);
			TIM_OC2Init(TIM2, &TIM_OCInitStructure);
			TIM_SetCompare2(TIM2, CCR);
		break;
	}
	TIM_Cmd(TIM2, ENABLE);
#endif
	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM2_PWM_Init(1, ARR, PSC, CCR);
			TIM2_PWM_Init(2, ARR, PSC, CCR);
		break;
		
		case 1:
			TIM_SetCompare1(TIM2, CCR);
		break;
		
		case 2:
			TIM_SetCompare2(TIM2, CCR);
		break;
	}
}
void TIM8_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR)
{
#ifndef __TIM_ALL_INIT
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);

	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR,
		.TIM_Prescaler = PSC,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_RepetitionCounter= 0,
	};
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM8_PWM_Init(1, ARR, PSC, CCR);
			TIM8_PWM_Init(2, ARR, PSC, CCR);
			TIM8_PWM_Init(3, ARR, PSC, CCR);
			TIM8_PWM_Init(4, ARR, PSC, CCR);
		break;
		
		case 1:
			SmpIO_Init(C6, AF_PP);
			TIM_OC1Init(TIM8, &TIM_OCInitStructure);
			TIM_SetCompare1(TIM8, CCR);
		break;
		
		case 2:
			SmpIO_Init(C7, AF_PP);
			TIM_OC2Init(TIM8, &TIM_OCInitStructure);
			TIM_SetCompare2(TIM8, CCR);
		break;
		
		case 3:
			SmpIO_Init(C8, AF_PP);
			TIM_OC3Init(TIM8, &TIM_OCInitStructure);
			TIM_SetCompare3(TIM8, CCR);
		break;
		
		case 4:
			SmpIO_Init(C9, AF_PP);
			TIM_OC4Init(TIM8, &TIM_OCInitStructure);
			TIM_SetCompare4(TIM8, CCR);
		break;
	}
	TIM_Cmd(TIM8, ENABLE);
	#endif
	switch(Chx)
	{
		case PWM_ALL_Ch:
			TIM8_PWM_Init(1, ARR, PSC, CCR);
			TIM8_PWM_Init(2, ARR, PSC, CCR);
			TIM8_PWM_Init(3, ARR, PSC, CCR);
			TIM8_PWM_Init(4, ARR, PSC, CCR);
		break;
		
		case 1:
			TIM_SetCompare1(TIM8, CCR);
		break;
		
		case 2:
			TIM_SetCompare2(TIM8, CCR);
		break;
		
		case 3:
			TIM_SetCompare3(TIM8, CCR);
		break;
		
		case 4:
			TIM_SetCompare4(TIM8, CCR);
		break;
	}
}
