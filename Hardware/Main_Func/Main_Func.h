#ifndef _TEST_H
#define _TEST_H

__Status Func_MainInit(void);
void Main_Func(void* arg);
void Temperature_Weighted_Cal(void);
double Func_TempToPID(double Temp);
double Func_PIDToAct(double Out);

extern USART_TypeDef* ChSW;

extern PID_Struct PID_TempALL;

extern float Temperature_OFBox[3];
#endif
