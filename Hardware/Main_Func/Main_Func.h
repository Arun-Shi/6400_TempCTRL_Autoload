#ifndef _TEST_H
#define _TEST_H

__Status Func_MainInit(void);
void Main_Func(void* arg);
void Func_Key_Run(u8 En);

void Temperature_Average_Cal(void);
double Func_TempToPID(double Temp);
double Func_PIDToActLeft(double Out);
double Func_PIDToActMid(double Out);
double Func_PIDToActRight(double Out);

extern USART_TypeDef* ChSW;

extern PID_Struct PID_TempLeft;
extern PID_Struct PID_TempMid;
extern PID_Struct PID_TempRight;
extern short __Value_MAXTemp;
extern float Temperature_OFBox[3];
#endif
