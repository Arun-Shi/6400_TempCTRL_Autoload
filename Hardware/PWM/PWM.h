#ifndef __PWM_H__
#define __PWM_H__

#include "./Base_Head_File.h"

#define PWM_OPEN	2
#define PWM_CLOSE	1

void PWM3_Operate(u8 Ope);
void PWM4_Operate(u8 Ope);
void PWM3_Chx_Set(u8 Chx, u32 Feq, int Duty);
void PWM4_Chx_Set(u8 Chx, u32 Feq, int Duty);
void PWM2_Chx_Set(u8 Chx, u32 Feq, int Duty);
void PWM8_Chx_Set(u8 Chx, u32 Feq, int Duty);

#endif
