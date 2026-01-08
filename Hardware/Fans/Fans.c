#include "./Fans/Fans.h"
Fan_Struct  Fans_ALL[14]={
	{0,0,0,Cross_Front},	{0,0,0,Cross_Rear},	{0,0,0,A_Side_L_D},	{0,0,0,A_Side_M_D},	{0,0,0,A_Side_R_D},	{0,0,0,A_Side_L_U},	{0,0,0,A_Side_M_U},
	{0,0,0,A_Side_R_U},		{0,0,0,B_Side_L_D},	{0,0,0,B_Side_M_D},	{0,0,0,B_Side_R_D},	{0,0,0,B_Side_L_U},	{0,0,0,B_Side_M_U},	{0,0,0,B_Side_R_U},
};
//设置输出功率
void Fans_ALL_Init(void)
{
#ifdef __TIM_ALL_INIT					//集中初始化标志，在C/C++栏宏定义
//IO配置
	SmpIO_Init(A6, AF_PP);SmpIO_Init(A7, AF_PP);SmpIO_Init(B0, AF_PP);SmpIO_Init(B1, AF_PP);		//PWM3
	SmpIO_Init(D12, AF_PP);SmpIO_Init(D13, AF_PP);SmpIO_Init(D14, AF_PP);SmpIO_Init(D15, AF_PP);	//PWM4
	SmpIO_Init(A15, AF_PP);SmpIO_Init(B3, AF_PP);															//PWM2
	SmpIO_Init(C6, AF_PP);SmpIO_Init(C7, AF_PP);SmpIO_Init(C8, AF_PP);SmpIO_Init(C9, AF_PP);		//PWM8
//时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
//定时器配置
	u16 Feq=25000;
	u32 ARR_T;
	u16 PSC_T;
	for(PSC_T=1;;)
	{
		ARR_T = (unsigned long)PWM3_F/(Feq*PSC_T);
		if(ARR_T>U16_MAX || PWM3_F% ARR_T !=0)
			PSC_T++;
		else
			break;
	}
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = ARR_T-1,
		.TIM_Prescaler = PSC_T-1,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_RepetitionCounter= 0,
	};
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
#endif
}
u8 Fans_Set_Chx_OutP(__Fans_Chx Chx, int Out_P)
{
	Out_P>100? Out_P=100:Out_P;
	Out_P<0? Out_P=0:Out_P;
	switch(Chx/2)
	{
		case 0:
		case 1:
			PWM3_Chx_Set((Chx%4)+1, Fans_PWM_Feq, Out_P);
		break;
		case 2:
		case 3:
			PWM4_Chx_Set((Chx%4)+1, Fans_PWM_Feq, Out_P);
		break;
		case 4:
			PWM2_Chx_Set((Chx%4)+1, Fans_PWM_Feq, Out_P);
		break;
		case 5:
		case 6:
			PWM8_Chx_Set((Chx%4)+1, Fans_PWM_Feq, Out_P);
		break;
		default: return FALSE;
	}
	return TRUE;
}
void Fans_ALL_Set_Duty(void)
{
	for(u8 index;index<__Fans_Chx_Num;index++)
		Fans_Set_Chx_OutP(index, Fans_ALL[index].Out_Duty);
}
//切换整体运行状态
u8 Fans_Set_Switch_Status(__Fans_Run_State Status)
{
	u8 STA=TRUE;
	switch(Status)
	{
		case Fans_Stop:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
				STA=Fans_Set_Chx_OutP(Chx, 0);
			break;
		
		case Fans_Run_Max:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
				STA=Fans_Set_Chx_OutP(Chx, 100);
			break;
		
		case Fans_Run_Dot8:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
				STA=Fans_Set_Chx_OutP(Chx, 80);
			break;
		
		case Fans_Run_Dot6:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
				STA=Fans_Set_Chx_OutP(Chx, 60);
			break;
		
		
		case Fans_Run_Dot3:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
				STA=Fans_Set_Chx_OutP(Chx, 30);
			break;
		
		case Fans_Run_Dissipation:
			for(u8 Chx=0; Chx<__Fans_Chx_Num; Chx++)
			{
				if(Chx== Cross_Rear)
					STA=Fans_Set_Chx_OutP(Chx, 0);
				else
					STA=Fans_Set_Chx_OutP(Chx, 70);
			}
		break;
		
		default:
			return FALSE;
	}
	return STA;
}
