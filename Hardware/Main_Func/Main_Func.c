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
//运行灯定义
	Pin_RLED1=E2,
	Pin_RLED2=E3,

//到位传感器定义引脚
	Pin_SenseUP=C3,
	Pin_SenseDOWN=A0,
	Pin_SenseIN=A4,
	Pin_SenseOUT=A5,

//步进电机相关引脚
	Pin_StepPulse_UD=F5,					//上下方向电机的脉冲引脚
	Pin_StepDir_UD=F7,
	Pin_StepEN_UD=F8,

	Pin_StepPulse_FB=F9,					//前后方向电机的脉冲引脚
	Pin_StepDir_FB=F10,
	Pin_StepEN_FB=C0,

	Pin_SW=B14,
}Func_IO;

volatile u8 Step_RunEN_UDU= TRUE;					//上下方向步进电机_上行动作使能
volatile u8 Step_RunEN_UDD= TRUE;					//上下方向步进电机_下行动作使能
volatile u8 Step_RunEN_FBF= TRUE;					//前后方向步进电机_前进动作使能
volatile u8 Step_RunEN_FBB= TRUE;					//前后方向步进电机_后退动作使能
#endif
/*****************************************************************************全局句柄***********************************************************************************/
#if 1
USART_TypeDef * ChSW=Ch1;
PID_Struct PID_TempLeft={
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
	.O_Change=Func_PIDToActLeft,
};
PID_Struct PID_TempMid={
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
	.O_Change=Func_PIDToActMid,
};
PID_Struct PID_TempRight={
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
	.O_Change=Func_PIDToActRight,
};

float Temperature_OFBox[3];						//温度数据，三舱室分开的话，需要使用三个，否则使用第一个
short __Value_MAXTemp=90;
u8 Stepper_RunEN=TRUE;
#define Stepper_LenTPulse  800;
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

	SmpIO_Init(Pin_RLED1,PP_0);						//运行灯初始化
	SmpIO_Init(Pin_RLED2,PP_1);
	SmpIO_Init(Pin_SW,IPU);							//切换脚初始化

	if(!sta)
		Printf_Chx(ChSW,"%s\r\n","初始化失败");
	return sta;
}
double Func_TempToPID(double Temp)
{
	return Temp*125;						//转化为PID输入，80->10000
}
double Func_PIDToActLeft(double Out)
{
	//占位，直接将输出值应用于执行器
	return Out/200;							//PID输出转化为功率比例，10000->50
}
double Func_PIDToActMid(double Out)
{
	//占位，直接将输出值应用于执行器
	return Out/200;							//PID输出转化为功率比例，10000->50
}
double Func_PIDToActRight(double Out)
{
	//占位，直接将输出值应用于执行器
	return Out/200;							//PID输出转化为功率比例，10000->50
}
//IO直接发送PWM
void Func_IOSend(u8 IO, u8 Speed)
{
	SmpIO_Toggle(IO);
	switch(Speed)
	{
		case 0: Delay_ms(1);		break;		//1KHz
		case 1: Delay_us(200);		break;		//5KHz
		case 2: Delay_us(20);		break;		//50KHz
		case 3: Delay_us(2);		break;		//500KHz
		// case 4: Delay_500ns();		break;		//2MHz->约1MHz
		default: return;
	}
}
//步进电机运行一段距离，内部包含有使能标志位，碰到到位开关时自己停止。
u8 Func_Stepper_Run(u8 Ch, u32 Len, u8 Dir, u8 Speed)
{
	u8 sta=FALSE;
	volatile u8* flag_en=NULL;								//方向使能标志引用
	u8 pin_pulse= Ch? Pin_StepPulse_UD: Pin_StepPulse_FB;	//当前通道的步进电机脉冲引脚
	u8 pin_dir= Ch? Pin_StepDir_UD: Pin_StepDir_FB;			//当前通道的步进电机方向引脚
	u32 pulse= Len*Stepper_LenTPulse;						//计算要发送的脉冲数

	if(Ch==1)
	{
		if(Dir)
			flag_en= &Step_RunEN_UDU;
		else 
			flag_en= &Step_RunEN_UDD;
	}
	else if(Ch==0)
	{
		if(Dir)
			flag_en= &Step_RunEN_FBF;
		else 
			flag_en= &Step_RunEN_FBB;
	}
	SmpIO_Set(pin_dir, Dir);					//设置方向信号

	while(pulse--)								//循环发送这么多个脉冲
		if(*flag_en)							//判断电机当前方向的使能，此标志位会在监管函数中一直轮询，实时更新到这里
			Func_IOSend(pin_pulse, Speed);
		else
			goto __FLAG_EXIT;
	sta=TRUE;									//全部脉冲数跑完，返回TRUE
	__FLAG_EXIT: return sta;
}
#define __Stepper_Len_UD 100					//步进电机_上下方向_行程
#define __Stepper_Len_FB 300					//步进电机_前后方向_行程
//执行动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
u8 Func_RunFlow(u8 Flow)					//0是退出流程，1是夹持流程
{
	static u8 step=0;						//状态机步骤索引
	
//开始执行退出动作流程（状态机）
	switch(step)							//状态机
	{
		case 0:								//检查所有传感器状态（是否互斥）
		{
			if(SmpIO_Read(Pin_SenseUP)&& SmpIO_Read(Pin_SenseDOWN))
				return FALSE;
			if(SmpIO_Read(Pin_SenseIN)&& SmpIO_Read(Pin_SenseOUT))
				return FALSE;
			step++;
			break;
		}
		case 1:								//下降
		{
			if(Func_Stepper_Run(1, __Stepper_Len_UD, Flow, 1)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(1, 10, Flow, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;														//还无法到位，则返回FALSE
			break;
		}
		case 3:								//后退，
		{
			if(Func_Stepper_Run(0, __Stepper_Len_FB, Flow, 0)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(0, 10, Flow, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;														//还无法到位，则返回FALSE
		}
		case 4:							//还原
		{
			step=0;
			return TRUE;
		}
	}
	return 0xFF;
}
//判断按键，执行操作
void Func_InPos_Judge(void)
{
		if(Stepper_RunEN)
			vTaskDelay(1);
		else
			return;								//直接返回
			
		Step_RunEN_UDU= !SmpIO_Read(Pin_SenseUP);
		Step_RunEN_UDD= !SmpIO_Read(Pin_SenseDOWN);
		Step_RunEN_FBF= !SmpIO_Read(Pin_SenseIN);
		Step_RunEN_FBB= !SmpIO_Read(Pin_SenseOUT);
}
void Func_Key_Run(u8 En)
{
	#define __Short_Judg	100				//短按判断时间阈值
	#define __Long_Judg		1500			//长按判断时间阈值
	#define BUT_L (SmpIO_Read(G12))			//双按左键状态
	#define BUT_R (SmpIO_Read(G15))			//双按右键状态

	if(!En)	return;							//不使能时退出

	static TickType_t index=0;				//按下的时间戳
	static u8 step=0;						//按键扫描步骤
	u8 DUBL=FALSE;							//双按标志，松手或超时时会判断
	u8 time=0;								//长短按，0：还没实现，1：短按，2：长按

	switch(step)							//按键扫描状态机
	{
		case 0: 							//等待任一按键按下
		{
			if(BUT_L || BUT_R)
			{
				step++;						//进入按下持续判断
				index=xTaskGetTickCount();	//记录按键按下时间
			}
			break;
		}
		case 1:								//按下持续判断
		{
			if(!(BUT_L || BUT_R))			//按键松开
			{
				step++;						//进入完成步
				break;						//提前退出
			}
			if(xTaskGetTickCount()-index > __Long_Judg)
			{
				if(BUT_L && BUT_R)			//双按成立
				time=2;						//长按成立
				step++;						//进入完成步
			}
			else if(xTaskGetTickCount()-index > __Short_Judg)
				time=1;						//短按成立
			break;							//退出
		}
		case 2:								//完成步
		{
			u8 t_sta=0;
			if(DUBL && (time==2))
			{
				Printf_Chx(Ch1,"SHOW:C-气缸开始闭合#");			//占位，调用整体动作函数-》闭合
				while(1)
				{
					t_sta=Func_RunFlow(1);						//执行动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
					if(t_sta!=0xFF)
						Printf_Chx(Ch1,"%s",t_sta?"成功#":"失败#");
				}
			}
			else if(time ==1 )
			{

				Printf_Chx(Ch1,"SHOW:C-气缸开始退出...#");			//占位，调用整体动作函数-》退出
				while(1)
				{
					t_sta=Func_RunFlow(0);						//执行动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
					if(t_sta!=0xFF)
						Printf_Chx(Ch1,"%s",t_sta?"成功#":"失败#");
				}
			}
			//此时已执行完成
			if(!(BUT_L || BUT_R))							//如果按键松开，初始化为等待步
			{
				DUBL=FALSE;
				time=0;
				step=0;										//进入等待步
			}
			break;
		}
	}
	vTaskDelay(1);
}
//温度平均值计算，根据温度值和测温位置计算单仓总温度，用于PID输入
void Temperature_Average_Cal(void)
{
	float temp=0.0;
	//左仓前四个传感器
	temp=0.0;
	for(u8 i=0;i<4;i++)
		temp+=DS_ALL_Arr[i].Tempdata;
	Temperature_OFBox[0]=temp/4;

	//中仓5~9,5个传感器
	temp=0.0;
	for(u8 i=4;i<9;i++)
		temp+=DS_ALL_Arr[i].Tempdata;
	Temperature_OFBox[1]=temp/5;

	//右仓后四个传感器
	temp=0.0;
	for(u8 i=9;i<13;i++)
		temp+=DS_ALL_Arr[i].Tempdata;
	Temperature_OFBox[2]=temp/4;
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
