#ifndef __DELAY_H
#define __DELAY_H

#include <stm32f10x.h>

void Delay_Init(void);
void Delay_us(uint16_t us);
void Delay_ms(uint16_t ms);

#define Delay_125ns()		{__asm{nop;nop;nop;nop;nop;nop;nop;nop;nop;}}
#define Delay_250ns()		{Delay_125ns();Delay_125ns();}
#define Delay_500ns()		{Delay_250ns();Delay_250ns();}


#endif
