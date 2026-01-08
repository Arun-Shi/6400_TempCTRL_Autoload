#include "./Base_Head_File.h"
/*
1、主要功能模块：通道开关，采集输出，状态监管。内部模块：指令接口，电信号采集
2、指令可以设定通道开关，可以获取某一通道的状态，可以设定连续输出通道状态
	单次输出为当即采集并输出，持续输出在此次基础上，设定发送间隔，在主循环轮询进行间隔判断和输出
3、指令可以设定状态监管，对某通道的电压电流电功率可单独设置上下限进行持续监管，可设定超限自关闭
	监管实现在持续采集时，采集完即对监管开关和数值状态进行判断，错误即刻输出
*/
/*****************************************************************************关键宏定义*********************************************************************************/
#if 1
typedef enum{
	Pin_TLED1=E2,
	Pin_TLED2=E3,
	Pin_SW=B14,
}Func_IO;

#endif
/*****************************************************************************全局句柄***********************************************************************************/
#if 1
USART_TypeDef * ChSW=Ch1;
PID_Struct PID_TempALL={
	.Agm={
		.Kp=100,
		.Ki=90,
		.Kd=0,
		.D_Before=0,
		.Kd_Filter=0,
		.I_Limit=0,
		.I_Sep=0,
		.Out_Acc=0,
		.Death_err=0,
	},
	.Level=Hi_Speed,
	.PID_Switch=FALSE,
	.I_Change=Func_TempToPID,
	.O_Change=Func_PIDToAct,
};
float Temperature_OFBox[3];						//温度数据，三舱室分开的话，需要使用三个，否则使用第一个
#define one_box TRUE							//如果使用一个温箱控制
#endif
/*****************************************************************************功能函数***********************************************************************************/
#if 1
__Status Func_MainInit(void)
{
	__Status sta=TRUE;
	Delay_Init();
	USARTx_Init(Ch1,115200);
	MyIIC_Init();
	DS18B20_Init();

	SmpIO_Init(Pin_TLED1,PP_0);						//运行灯初始化
	SmpIO_Init(Pin_TLED2,PP_1);
	SmpIO_Init(Pin_SW,IPU);							//切换脚初始化

	if(!sta)
		Printf_Chx(ChSW,"%s\r\n","初始化失败");
	return sta;
}
double Func_TempToPID(double Temp)
{
	return Temp*125;						//转化为PID输入，80->10000
}
double Func_PIDToAct(double Out)
{
	//直接将输出值应用于执行器
	return Out/200;							//PID输出转化为功率比例，10000->50
}
//温度加权计算，根据温度值和测温位置计算总温度，用于PID输入
void Temperature_Weighted_Cal(void)
{
	u8 mid_factor=30;												//中间系数值
	if(one_box)
	{
		float side_factor=(100-mid_factor)/(__MAX_DS_NUM-1);		//获取边角系数值
		Temperature_OFBox[0]=0;
		Temperature_OFBox[0]+=DS_ALL_Arr[0].Tempdata* mid_factor;	//加上中间值
		for(u8 i=1;i<__MAX_DS_NUM;i++)								//加上边角值
			Temperature_OFBox[0]+=DS_ALL_Arr[i].Tempdata* side_factor;
	}
	//else分仓计算，各自计算加权，分出三个参考温度值
}
#endif
/*****************************************************************************中断处理***********************************************************************************/
#if 1
//秒中断
void RTC_IRQHandler(void)
{

	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{

		RTC_ClearITPendingBit(RTC_IT_ALR);
	}
}

#endif
