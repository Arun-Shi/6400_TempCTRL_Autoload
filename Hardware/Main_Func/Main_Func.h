#ifndef _TEST_H
#define _TEST_H
typedef enum{
    _PWMOrd_HeatLeft= 0,           //控制加热丝的PWM通道
    _PWMOrd_HeatMidL,
    _PWMOrd_HeatMidR,
    _PWMOrd_HeatRight,

    _PWMOrd_FanHL,                 //左侧加热丝后的风扇
    _PWMOrd_FanHMidL,
    _PWMOrd_FanHMidR,
    _PWMOrd_FanHR,

    _PWMOrd_FanOutL,                //左仓出口处的风扇
    _PWMOrd_FanOutMidL,
    _PWMOrd_FanOutMidR,
    _PWMOrd_FanOutR,
}__PWM_Ord;

__Status Func_MainInit(void);
void Main_Func(void* arg);
void Func_Key_Run(u8 En);

void Temperature_Average_Cal(void);
float Func_TempToPID(float Temp);
float Func_PIDToActLeft(float Out);
float Func_PIDToActMid(float Out);
float Func_PIDToActRight(float Out);
void Func_InPos_Judge(void);
void Func_Dissipation(void);
void Func_Standby(void);



extern USART_TypeDef* ChSW;

extern PID_Struct PID_TempLeft;
extern PID_Struct PID_TempMid;
extern PID_Struct PID_TempRight;
extern short __Value_MAXTemp;
extern float Temperature_OFBox[3];
extern u8 Stepper_RunEN;
#endif
