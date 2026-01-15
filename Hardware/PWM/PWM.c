#include "./PWM/PWM.h"
#define _FansPWM_freq		25000
#define _FansPWM_Arr		(1000-1)
#define _FansPWM_Pscal		(2880-1)

#define _HeatPWM_freq		1
#define _HeatPWM_Arr		(10000-1)
#define _HeatPWM_Pscal		(7200-1)
void TIM_PWMs_Init(void)
{
//IO配置
	SmpIO_Init(E14,AF_PP);	SmpIO_Init(E13,AF_PP);	SmpIO_Init(E11,AF_PP); SmpIO_Init(E9,AF_PP);
	SmpIO_Init(B1,AF_PP);	SmpIO_Init(B0,AF_PP);	SmpIO_Init(A7,AF_PP);	SmpIO_Init(A6,AF_PP);	
	SmpIO_Init(D15,AF_PP);	SmpIO_Init(D14,AF_PP);	SmpIO_Init(D13,AF_PP);	SmpIO_Init(D13,AF_PP);	
//时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			//1Hz
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//25kHz
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//25kHz
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
//定时器配置
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={
		.TIM_Period = _FansPWM_Arr,
		.TIM_Prescaler = _FansPWM_Pscal,
		.TIM_ClockDivision = TIM_CKD_DIV1,
		.TIM_CounterMode = TIM_CounterMode_Up,
		.TIM_RepetitionCounter= 0,
	};
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Period=_HeatPWM_Arr;
	TIM_TimeBaseStructure.TIM_Prescaler = _HeatPWM_Arr;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);									//赋值一些关键属性，CCR默认为0，不打开
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}
//设置PWM波的占空比，Pos是顺序，按照接线图设定的加热丝顺序(0~3)+风扇顺序(4~11)去执行。
void PWM_SetDuty(u8 Pos, u8 Duty)
{
	if(Pos>11 || Duty>100)
		return;												//限定参数的范围
	TIM_TypeDef* timx[]={TIM1,TIM3,TIM4};					//定时器指针
	//函数指针数组形式，排列赋值一个CCR赋值函数数组，方便使用
	void(*(ccr_ope[]))(TIM_TypeDef*, u16 )={TIM_SetCompare1, TIM_SetCompare2, TIM_SetCompare3, TIM_SetCompare4};
	u16 pos_arr= (Pos/4)?_FansPWM_Arr:_HeatPWM_Arr;			//0~3为发热丝，使用不同的装载值

	if(Duty==FALSE)
		ccr_ope[Pos%4](timx[Pos/4], 0);						//相应CCR赋值0
	else
		ccr_ope[Pos%4](timx[Pos/4], pos_arr/100.0*Duty);	//相应CCR赋值为 pos_arr* Duty%
}
