#include "./delay/delay.h"
void Delay_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	TIM_Cmd(TIM6, ENABLE);
}
void Delay_ms(uint16_t ms)
{
	uint32_t i;
	for(i=0; i<ms; i++)
		Delay_us(999);

}
void Delay_us(uint16_t us)
{
	TIM_SetCounter(TIM6, 0);
	while(TIM6->CNT < us);
}

