#include "./Base_Head_File.h"
/*****************************************************************************关键宏定义*********************************************************************************/
#if 1
typedef enum{
//运行灯定义
	Pin_RLED1=E2,
	Pin_RLED2=E3,

//到位传感器定义引脚
	Pin_Sense_UDU=C3,						//上下行程	上传感器
	Pin_Sense_UDD=A0,						//上下行程  下传感器
	Pin_Sense_FBB=A4,						//前后行程  前传感器
	Pin_Sense_FBF=A5,						//前后行程  后传感器

//步进电机相关引脚
	Pin_StepPulse_UD=F5,					//上下方向电机的脉冲引脚	非定时器引脚，使用IO翻转模拟脉冲
	Pin_StepDir_UD=F7,						//上下方向电机的方向引脚
	Pin_StepEN_UD=F8,						//上下方向电机的使能引脚

	Pin_StepPulse_FB=F9,					//前后方向电机的脉冲引脚	非定时器引脚，使用IO翻转模拟脉冲
	Pin_StepDir_FB=F10,						//前后方向电机的方向引脚
	Pin_StepEN_FB=C0,						//前后方向电机的使能引脚

//发热丝相关引脚
	Pin_Heating_Right=E14,					//右仓发热丝引脚			Tim1_CH4
	Pin_Heating_MidR=E13,					//中仓右发热丝引脚			Tim1_CH3
	Pin_Heating_MidL=E11,					//中仓左发热丝引脚			Tim1_CH2
	Pin_Heating_Left=E9,					//左仓发热丝引脚			Tim1_CH1

//空气循环风扇引脚
	Pin_FanPWM_HR=B1,						//空气循环风扇PWM引脚_右仓发热丝	TIM3_CH4
	Pin_FanPWM_HMidR=B0,					//空气循环风扇PWM引脚_中仓右发热丝	TIM3_CH3
	Pin_FanPWM_HMidL=A7,					//空气循环风扇PWM引脚_中仓左发热丝  TIM3_CH2
	Pin_FanPWM_HL=A6,						//空气循环风扇PWM引脚_左仓发热丝  	TIM3_CH1
	Pin_FanPWM_OutR=D15,					//空气循环风扇PWM引脚_右仓出口		Tim4_CH4
	Pin_FanPWM_OutMidR=D14,					//空气循环风扇PWM引脚_中仓右出口	Tim4_CH3
	Pin_FanPWM_OutMidL=D13,					//空气循环风扇PWM引脚_中仓左出口	Tim4_CH2
	Pin_FanPWM_OutL=D12,					//空气循环风扇PWM引脚_左仓出口		Tim4_CH1

//散热电推杆控制引脚
	Pin_Windows_BackL1= E2,					//电推杆——左仓后面引脚1
	Pin_Windows_BackL2= E3,					//电推杆——左仓后面引脚2
	Pin_Windows_BackR1= E4,					//电推杆——右仓后面引脚1
	Pin_Windows_BackR2= E5,					//电推杆——右仓后面引脚2
	Pin_Windows_FrontL1= E6,				//电推杆——左仓前面引脚1
	Pin_Windows_FrontL2= F0,				//电推杆——左仓前面引脚2
	Pin_Windows_FrontR1= F1,				//电推杆——右仓前面引脚1
	Pin_Windows_FrontR2= F2,				//电推杆——右仓前面引脚2

//温度传感器引脚
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
float Func_TempToPID(float Temp)
{
	return Temp*125;								//转化为PID输入，80->10000
}
//散热窗控制权设计理念：开启优先级大于关闭，想开就开，但控制权自由才可获取，想关得判断控制权，关闭后释放控制权
u8 xSema_WindowCTRL_LM	=0;							//模拟信号量机制，开窗置1或2，判断为自己的控制权时，允许关窗置0	L为1，M为2，R为3
u8 xSema_WindowCTRL_RM	=0;							//模拟信号量机制，开窗置2或3，判断为自己的控制权时，允许关窗置0
//执行器包括热循环扇（输出的绝对值控制占空比，即转速)、加热丝（输出值（正数）控制占空比）、散热窗（输出低于-10则开启散热窗）
float Func_PIDToActLeft(float Out)					//PID输出转化为功率比例，10000->50
{
	Out/=200;
	//加热循环风扇转速
	PWM_SetDuty(_PWMOrd_FanHL,fabs(Out));

	//设定发热丝的功率
	if(__Flt_Great(Out,0))
		PWM_SetDuty(_PWMOrd_HeatLeft,Out);
	else
		PWM_SetDuty(_PWMOrd_HeatLeft,0);
	//设定散热窗
	if(__Flt_Less(Out,-10))
	{
		PWM_SetDuty(_PWMOrd_FanOutL,fabs(Out));		//打开散热风扇，打开散热窗
		if(xSema_WindowCTRL_LM==0)
		{
			xSema_WindowCTRL_LM=1;					//左中窗口 标记为左仓控制
			SmpIO_Set(Pin_Windows_FrontL1, TRUE);
			SmpIO_Set(Pin_Windows_FrontL2, FALSE);
		}
	}
	else
	{
		PWM_SetDuty(_PWMOrd_FanOutL,0);				//关闭散热风扇，关闭散热窗
		if(xSema_WindowCTRL_LM==1)					//左中窗口控制权在手时，可关闭
		{
			SmpIO_Set(Pin_Windows_FrontL1, FALSE);
			SmpIO_Set(Pin_Windows_FrontL2, TRUE);
			xSema_WindowCTRL_LM=0;
		}
	}
	return Out;
}
float Func_PIDToActMid(float Out)
{
	Out/=200;
	//加热循环风扇转速
	PWM_SetDuty(_PWMOrd_FanHMidL,fabs(Out));
	PWM_SetDuty(_PWMOrd_FanHMidR,fabs(Out));

	//设定发热丝的功率
	if(__Flt_Great(Out,0))
	{
		PWM_SetDuty(_PWMOrd_HeatMidL,Out);
		PWM_SetDuty(_PWMOrd_HeatMidR,Out);
	}
	else
	{
		PWM_SetDuty(_PWMOrd_HeatMidL,0);
		PWM_SetDuty(_PWMOrd_HeatMidR,0);
	}
	//设定散热窗
	if(__Flt_Less(Out,-10))
	{
		PWM_SetDuty(_PWMOrd_FanOutL,fabs(Out));		//打开散热风扇，打开散热窗
		if(xSema_WindowCTRL_LM==0)
		{
			xSema_WindowCTRL_LM=2;					//左中窗口 标记为中仓控制
			SmpIO_Set(Pin_Windows_FrontL1, TRUE);
			SmpIO_Set(Pin_Windows_FrontL2, FALSE);
		}
		if(xSema_WindowCTRL_RM==0)
		{
			xSema_WindowCTRL_RM=2;					//右中窗口 标记为中仓控制
			SmpIO_Set(Pin_Windows_FrontR1, TRUE);
			SmpIO_Set(Pin_Windows_FrontR2, FALSE);
		}
	}
	else
	{
		PWM_SetDuty(_PWMOrd_FanOutL,0);				//关闭散热风扇，关闭散热窗
		if(xSema_WindowCTRL_LM==2)					//左中窗口控制权在手时，可关闭
		{
			SmpIO_Set(Pin_Windows_FrontL1, FALSE);
			SmpIO_Set(Pin_Windows_FrontL2, TRUE);
			xSema_WindowCTRL_LM=0;
		}
		if(xSema_WindowCTRL_RM==2)					//右中窗口控制权在手时，可关闭
		{
			SmpIO_Set(Pin_Windows_FrontR1, FALSE);
			SmpIO_Set(Pin_Windows_FrontR2, TRUE);
			xSema_WindowCTRL_RM=0;
		}
	}
	return Out;
}
float Func_PIDToActRight(float Out)
{
	Out/=200;
	//加热循环风扇转速
	PWM_SetDuty(_PWMOrd_FanHR,fabs(Out));

	//设定发热丝的功率
	if(__Flt_Great(Out,0))
		PWM_SetDuty(_PWMOrd_HeatRight,Out);
	else
		PWM_SetDuty(_PWMOrd_HeatRight,0);
	//设定散热窗
	if(__Flt_Less(Out,-10))
	{
		PWM_SetDuty(_PWMOrd_FanOutR,fabs(Out));		//打开散热风扇，打开散热窗
		if(xSema_WindowCTRL_RM==0)
		{
			xSema_WindowCTRL_RM=3;					//左中窗口 标记为左仓控制
			SmpIO_Set(Pin_Windows_FrontR1, TRUE);
			SmpIO_Set(Pin_Windows_FrontR2, FALSE);
		}
	}
	else
	{
		PWM_SetDuty(_PWMOrd_FanOutR,0);				//关闭散热风扇，关闭散热窗
		if(xSema_WindowCTRL_RM==3)					//左中窗口控制权在手时，可关闭
		{
			SmpIO_Set(Pin_Windows_FrontR1, FALSE);
			SmpIO_Set(Pin_Windows_FrontR2, TRUE);
			xSema_WindowCTRL_RM=0;
		}
	}
	return Out;
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

//执行产品退出动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
u8 Func_RunExit(u8 En)
{
	static u8 step=0;						//状态机步骤索引
	
//开始执行退出动作流程（状态机）
	switch(step)							//状态机
	{
		case 0:								//检查所有传感器状态（是否互斥）
		{
			if(SmpIO_Read(Pin_Sense_UDU)&& SmpIO_Read(Pin_Sense_UDD))
				return FALSE;
			if(SmpIO_Read(Pin_Sense_FBF)&& SmpIO_Read(Pin_Sense_FBB))
				return FALSE;
			step++;
			break;
		}
		case 1:								//下降
		{
			if(Func_Stepper_Run(1, __Stepper_Len_UD, 0, 1)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(1, 10, 0, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;													//还无法到位，则返回FALSE
			break;
		}
		case 3:								//后退，
		{
			if(Func_Stepper_Run(0, __Stepper_Len_FB, 0, 0)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(0, 10, 0, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;													//还无法到位，则返回FALSE
		}
		case 4:								//还原
		{
			step=0;
			return TRUE;
		}
	}
	return 0xFF;
}
//执行产品夹持动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
u8 Func_RunClamp(u8 En)
{
	static u8 step=0;						//状态机步骤索引
//开始执行退出动作流程（状态机）
	switch(step)							//状态机
	{
		case 0:								//检查所有传感器状态（是否互斥）
		{
			if(SmpIO_Read(Pin_Sense_UDU)&& SmpIO_Read(Pin_Sense_UDD))
				return FALSE;
			if(SmpIO_Read(Pin_Sense_FBF)&& SmpIO_Read(Pin_Sense_FBB))
				return FALSE;
			step++;
			break;
		}
		case 1:								//下降
		{
			if(Func_Stepper_Run(1, __Stepper_Len_UD, 0, 1)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(1, 10, 0, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;													//还无法到位，则返回FALSE
			break;
		}
		case 3:								//推进
		{
			if(Func_Stepper_Run(0, __Stepper_Len_FB, 1, 0)==FALSE)				//上下电机慢速推进，到位会停止
				step++;
			else if(Func_Stepper_Run(0, 10, 1, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;													//还无法到位，则返回FALSE
		}
		case 4:								//上升压合
		{
			if(Func_Stepper_Run(1, __Stepper_Len_UD, 1, 1)==FALSE)				//上下电机慢速下降，到位会停止
				step++;
			else if(Func_Stepper_Run(1, 10, 1, 0)==FALSE)						//还没到位时，再增加10mm
				step++;
			else return FALSE;													//还无法到位，则返回FALSE
			break;
		}
		case 5:								//还原
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
	{
		Step_RunEN_UDU= !SmpIO_Read(Pin_Sense_UDU);
		Step_RunEN_UDD= !SmpIO_Read(Pin_Sense_UDD);
		Step_RunEN_FBF= !SmpIO_Read(Pin_Sense_FBF);
		Step_RunEN_FBB= !SmpIO_Read(Pin_Sense_FBB);
	}
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
				Printf_Chx(Ch1,"SHOW:C-气缸开始闭合#");				//调用整体动作函数-》闭合
				while(1)
				{
					t_sta=Func_RunClamp(Stepper_RunEN);				//执行动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
					if(t_sta!=0xFF)
						Printf_Chx(Ch1,"%s",t_sta?"成功#":"失败#");
				}
			}
			else if(time ==1 )
			{

				Printf_Chx(Ch1,"SHOW:C-气缸开始退出...#");			//调用整体动作函数-》退出
				while(1)
				{
					t_sta=Func_RunExit(Stepper_RunEN);				//执行动作流程，返回状态，TRUE为成功，FALSE为失败，0xFF是正在运行
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
