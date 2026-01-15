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


#endif
