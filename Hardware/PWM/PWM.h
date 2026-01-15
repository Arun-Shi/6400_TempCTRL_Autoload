#ifndef __PWM_H__
#define __PWM_H__

#include "./Base_Head_File.h"

#define PWM_OPEN	2
#define PWM_CLOSE	1

void TIM_PWMs_Init(void);
void PWM_SetDuty(u8 Pos, u8 Duty);

#endif
