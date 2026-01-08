#ifndef _PID_H
#define _PID_H

typedef enum _Perform_Level
{
	LOW_Speed=0,							//低速——适用于整定时间 分钟级
	Mid_Speed,								//中速——适用于整定时间 秒级
	Hi_Speed,								//高速——适用于整定时间 百毫秒级
	Slave_Speed,							//伺服——适用于整定时间 毫秒级
}Perform_Level;								//PID调节系统性能等级,根据实际系统的调节速度来选择，高速通常需要高功率的输出能力和更高的调节频率，同时也会减小误差带

typedef struct PID_Agm{
	float I_Sep;							//积分分离
	float I_Limit;							//积分限幅
	float Out_Acc;							//输出值的最大每次变化量
	float D_Before;							//微分先行
	float Kp;								//比例系数
	float Ki;								//积分系数
	float Kd;								//微分系数
	float Death_err;						//死区
	float Kd_Filter;						//微分滤波器系数
}PID_Agm;									//PID参数结构体

typedef struct PID_Value{
	float Measure;							//测量值
	float L_Measure;						//上次测量
	float Error;							//误差值
	float L_Error;							//上次误差
	float Target;							//目标值
	float P_Out;							//比例输出值
	float I_Out;							//积分输出值
	float D_Out;							//微分输出值
	float T_Out;							//总输出值
	float Min_Out;							//最小输出值
	float Max_Out;							//最大输出值
}PID_Value;									//PID数值结构体

typedef struct PID_Struct{
	double (*I_Change)(double);				//输入单位转化函数指针
	double (*O_Change)(double);				//输出单位转化函数，目前设计为接口，内部直接将值应用于执行器
	PID_Value Value;						//PID数值结构体
	PID_Agm Agm;							//PID参数结构体
	Perform_Level Level;					//性能等级

	u8 PID_Switch;							//PID开关
}PID_Struct;

u8 PID_Struct_Init(PID_Struct* PID, PID_Value* Value, Perform_Level Level);
u8 PID_Switch(PID_Struct* PID, __Status Status);
u8 PID_Calculate_Out(PID_Struct* PID);
u8 PID_Set_Target(PID_Struct* PID, float Target);
u8 PID_Input_Change(PID_Struct* PID, float Value);
u8 PID_Apply_Out(PID_Struct* PID);

// u8 PID_Get_Value(PID_Struct* PID, PID_Value* Value);
// u8 PID_Set_Value(PID_Struct* PID, PID_Value* Value);
// u8 PID_Get_Agm(PID_Struct* PID, PID_Agm* Agm);
// u8 PID_Set_Agm(PID_Struct* PID, PID_Agm* Agm);
// u8 PID_Self_Adjust(PID_Struct* PID);
// void PID_Base_Timer_Init(u16 arr,u16 psc);							//PID控制器基础时钟

#endif
