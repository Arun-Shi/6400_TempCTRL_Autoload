#ifndef __TIMER_H
#define __TIMER_H

#include "./Base_Head_File.h"

#define PWM_ALL_Ch 0

#define Sys_Tim_Feq	72000000ul
#define PWM3_F Tim3_Src_Frequency
#define PWM4_F Tim4_Src_Frequency
#define Tim3_Src_Frequency 72000000ul
#define Tim4_Src_Frequency 72000000ul

void Sys_Base_TIM_Init(u16 Feq);
void TIM3_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR);
void TIM4_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR);
void TIM2_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR);
void TIM8_PWM_Init(u8 Chx, u16 ARR, u16 PSC, u16 CCR);



#endif
