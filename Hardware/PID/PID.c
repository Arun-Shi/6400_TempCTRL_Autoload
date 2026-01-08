#include "./Base_Head_File.h"
/****************************************************************************
1、PID源文件，提供PID初始化，自调整，控制计算等标准接口。
2、接口输出控制值，需要提供目标转换单位，并且提供稳定时钟并调用PID_Calculate_Out
3、PID控制器配置使用过程：定义PID_Struct并初始化->	开启PID控制器->	使用稳定时钟调用PID_Calculate_Out-> 以计算输出值作为依据调节系统输出

V1.1
1、增加死区和微分滤波参数
2、去除AGM数组和Value数组的获取和赋值函数
3、建议不使用PID_Struct_Init和Perform_Level_Agm
*****************************************************************************/
PID_Agm Perform_Level_Agm[4]={
	{100,		90,			0,		0,		3,		15,		0},			//低速
	{300,		250,		0,		0,		6,		2,		0},			//中速
	{400,		300,		0,		0,		8,		2,		2},			//高速
	{500,		400,		1,		0,		11,		3,		2},			//伺服
};																		//不同性能PID调控等级对应的PID初始参数,以1K作为最大输出(实际控制性能越强的系统，最大输出值也会越大，此数值仅供参考)
/*
函	数：PID结构体初始化
参	数：PID结构体指针
		Level- 性能等级
		Value- 数值结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：
实	现：
*/
u8 PID_Struct_Init(PID_Struct* PID, PID_Value* Value, Perform_Level Level)
{
	if(PID==NULL)
		return FALSE;
	PID->Level = Level;
	PID->Agm= Perform_Level_Agm[Level];							//整体赋值
	PID->Value= *Value;											//整体赋值
	return TRUE;
}
/*
函	数：PID开关
参	数：PID结构体指针
		status- 将更改为此状态
返	回：TRUE- 成功
		FALSE- 失败
备	注：Status传入FALSE为对PID控制器进行直接锁定
实	现：更改Switch，calculate函数将会判断该参数
*/
u8 PID_Switch(PID_Struct* PID, __Status Status)
{
	if(PID==NULL)
		return FALSE;
	PID->PID_Switch= Status;
	return TRUE;
}
/*
函	数：PID计算输出值
参	数：PID结构体指针
返	回：TRUE- 计算完成并输出成功
		FALSE- 计算失败，PID控制器锁定，或指针为空
备	注：此函数没有进行计算周期的判断，需要提供稳定时钟并调用此函数
实	现：根据PID结构体中的参数和数值进行计算
*/
u8 PID_Calculate_Out(PID_Struct* PID)
{
	int calc_result;
	if(PID==NULL || PID->PID_Switch == FALSE)
		return FALSE;

	if(PID->Agm.Death_err && __ABS(PID->Value.Error) < PID->Agm.Death_err)			//判断死区参数，如果死区参数存在，则判断误差值是否小于死区参数，小于则不进行计算
		return TRUE;

	PID->Value.Error= PID->Value.Target - PID->Value.Measure;
	
	PID->Value.P_Out= PID->Agm.Kp* PID->Value.Error;								//比例环节计算
	
	if(!(PID->Agm.I_Sep && (__ABS(PID->Value.Error) > PID->Agm.I_Sep)) )			//积分环节计算，!(判断积分分离开启并且，误差值大于积分分离值)，进行积分累计，否则不进行累计
		PID->Value.I_Out+= PID->Agm.Ki* PID->Value.Error;

	if(PID->Agm.I_Limit)															//如果存在积分限幅，将积分进行限幅比较
		PID->Value.I_Out= __Limit(PID->Value.I_Out, PID->Agm.I_Limit);
	
	float temp_d=PID->Value.D_Out;													//保存上一次的微分环节计算值
	if(PID->Agm.D_Before)															//微分环节计算，判断微分先行参数，存在则开启微分分离（微分系数为0，则不开启微分）
		PID->Value.D_Out= -PID->Agm.Kd* (PID->Value.Measure - PID->Value.L_Measure);//以测量值作为微分计算值，此时微分系数应是负数
	else
		PID->Value.D_Out= PID->Agm.Kd* (PID->Value.Error - PID->Value.L_Error);		//以误差值作为微分计算值
	
	if(PID->Agm.Kd_Filter)															//判断是否存在微分滤波参数，存在则开启微分滤波
		PID->Value.D_Out = (PID->Agm.Kd_Filter * temp_d) + (1.0 - PID->Agm.Kd_Filter) * PID->Value.D_Out;

	calc_result= PID->Value.P_Out+ PID->Value.I_Out+ PID->Value.D_Out;				//计算总输出

	calc_result= __Range_OF(calc_result, PID->Value.Min_Out, PID->Value.Max_Out); 	//限制输出值的大小

	if(PID->Agm.Out_Acc )//判断是否存在加速度限制，存在的话，限制此次变化量不大于Out_Acc
		calc_result= __Range_OF(calc_result, PID->Value.T_Out- PID->Agm.Out_Acc, PID->Value.T_Out+ PID->Agm.Out_Acc);

	PID->Value.T_Out= calc_result;

	return TRUE;
}
/*
函	数：PID设置目标值
参	数：PID结构体指针
		Target- 目标值
返	回：TRUE- 成功
		FALSE- 失败
备	注：目标值会依据单位转换函数进行转换后赋值给PID控制器

*/
u8 PID_Set_Target(PID_Struct* PID, int Target)
{
	if(PID==NULL)
		return FALSE;
	PID->Value.Target= PID->I_Change(Target);
	return TRUE;
}
/*
函	数：PID输出值应用
参	数：PID结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：调用PID结构体的输出函数，输出给控制器
*/
u8 PID_Apply_Out(PID_Struct* PID)
{
	if(PID==NULL)
		return FALSE;
	PID->O_Change(PID->Value.T_Out);
	return TRUE;
}
/***********************************下面的没用上*****************************/
#if 1
/*
函	数：PID参数自整定
参	数：PID结构体指针
返	回：
备	注：
实	现：
*/
u8 PID_Self_Adjust(PID_Struct* PID)
{
	//暂空，积累经验后编写
	return TRUE;
}
void PID_Base_Timer_Init(u16 arr,u16 psc)							//PID控制器基础时钟
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr; 						
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 						
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4, ENABLE);
}
/*
函	数：PID获取数值结构体
参	数：PID结构体指针
		Value- 数值结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：
实	现：
*/
u8 PID_Get_Value(PID_Struct* PID, PID_Value* Value)
{
	if(PID==NULL || Value==NULL)
		return FALSE;
	*Value= PID->Value;
	return TRUE;
}
/*
函	数：赋值PID数值结构体
参	数：PID结构体指针
		Value- 数值结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：
实	现：
*/
u8 PID_Set_Value(PID_Struct* PID, PID_Value* Value)
{
	if(PID==NULL || Value==NULL)
		return FALSE;
	PID->Value=*Value;
	return TRUE;
}
/*
函	数：PID获取参数结构体
参	数：PID结构体指针
		Agm- 数值结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：
实	现：
*/
u8 PID_Get_Agm(PID_Struct* PID, PID_Agm* Agm)
{
	if(PID==NULL || Agm==NULL)
		return FALSE;
	*Agm=PID->Agm;
	return TRUE;
}
/*
函	数：赋值PID参数结构体
参	数：PID结构体指针
		Agm- 数值结构体指针
返	回：TRUE- 成功
		FALSE- 失败
备	注：
实	现：
*/
u8 PID_Set_Agm(PID_Struct* PID, PID_Agm* Agm)
{
	if(PID==NULL || Agm==NULL)
		return FALSE;
	PID->Agm=*Agm;
	return TRUE;
}
#endif
