#include "RTOS_Task.h"
/************************全局变量、句柄****************************/
TaskHandle_t 	TCB_MyIdle;											//空闲任务
UBaseType_t		uxPriority_MyIdle=	0;

TaskHandle_t	TCB_UartCMD_Handler;								//串口指令处理任务
UBaseType_t		uxPriority_UartCMD_Handler= 0;

TaskHandle_t	TCB_PIDFunc_Handler;								//PID处理任务
UBaseType_t		uxPriority_PIDFunc_Handler= 3;

TaskHandle_t	TCB_CycleGetTemp;									//周期获取温度任务
UBaseType_t		uxPriority_CycleGetTemp_Handler= 2;

TaskHandle_t	TCB_SysSupervisor;									//系统状态监控任务
UBaseType_t		uxPriority_SysSupervisor= 4;

TaskHandle_t	TCB_Moving;											//机械动作任务
UBaseType_t		uxPriority_Moving= 3;

SemaphoreHandle_t xMutex_UartTX;									//打印端口的互斥量
SemaphoreHandle_t xMutex_IICBus;									//IIC总线互斥量
SemaphoreHandle_t xSem_UartRx;										//接收端口的信号量
/*************************任务集合**************************** */
/*******************************************************************************
 * @brief 	项目空闲任务：1、运行灯翻转	2、eep存储	3、
 * @return 	无
 * @note 	
 ******************************************************************************/
void vTask_MyIdle( void* arg )
{
	while(1)
	{
		__Cycle_ToDo(SmpIO_Toggle(E2);SmpIO_Toggle(E3),	5);		//每500ms执行一次灯翻转

		size_t temp= xPortGetFreeHeapSize();
//		__Cycle_ToDo(EEP参数保存,	100);						//每10s执行一次参数保存
		vTaskDelay(100);
	}
}
/*******************************************************************************
 * @brief 	串口指令解析任务
 * @return 	无
 * @note 	循环判断是否接受到完整串口指令，是则开始执行
 ******************************************************************************/
void vTask_UartCMD_Handler(void* arg)
{
	while(1)
	{
		//阻塞等待信号量
        if(xSemaphoreTake(xSem_UartRx, portMAX_DELAY) == pdTRUE)
            Command_Call();
	}
}
/*******************************************************************************
 * @brief 	PID相关任务
 * @return 	无
 * @note 	负责完成PID相关的任务，包括输入变换，PID计算，输出变换
 ******************************************************************************/
void vTask_PIDFunc_Handler(void* arg)
{
	TickType_t pxLastWakeTime= xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&pxLastWakeTime,500);
		//每500ms执行一次PID计算
		if(PID_Calculate_Out(&PID_TempALL))
			PID_Apply_Out(&PID_TempALL);
	}
}
void vTask_CycleGetTemp(void* arg)
{
	TickType_t pxLastWakeTime= xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&pxLastWakeTime,500);
		//每500ms执行一次温度获取

		DS18B20_ALL_Start_Convert();
		
		DS18B20_ALL_Read_Temp_float();
	}
}
void vTask_SysSupervisor(void* arg)
{
	TickType_t pxLastWakeTime= xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&pxLastWakeTime,10);
		//每10ms执行一次系统状态监控
	}
}
void vTask_Moving(void* arg)
{
	TickType_t pxLastWakeTime= xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&pxLastWakeTime,10);
		//每10ms进入机械动作函数
	}
}
void vTask_StartOS(void* arg)
{
//关闭任务切换
	vTaskSuspendAll();
//初始化资源
	xMutex_UartTX= xSemaphoreCreateMutex();						//创建打印输出的互斥锁
	xSem_UartRx= xSemaphoreCreateBinary();						//创建串口接收信号量
	xMutex_IICBus= xSemaphoreCreateMutex();						//创建IIC总线互斥锁

//初始化功能外设
	Main_FuncInit();

/****************************创建任务********************************/
//创建并运行空闲处理任务
	xTaskCreate(vTask_MyIdle, "MyIdle", 512, NULL, uxPriority_MyIdle, &TCB_MyIdle);
//创建并运行串口指令处理任务
	xTaskCreate(vTask_UartCMD_Handler, "UartCMD_Handler", 512, NULL, uxPriority_UartCMD_Handler, &TCB_UartCMD_Handler);
//创建并运行PID处理任务
	xTaskCreate(vTask_PIDFunc_Handler, "PIDFunc_Handler", 512, NULL, uxPriority_PIDFunc_Handler, &TCB_PIDFunc_Handler);	
//创建并运行温感矩阵任务
	xTaskCreate(vTask_CycleGetTemp, "CycleGetTemp", 512, NULL, uxPriority_CycleGetTemp_Handler, &TCB_CycleGetTemp);
//创建状态监控，报警任务
	xTaskCreate(vTask_SysSupervisor, "SysSupervisor", 512, NULL, uxPriority_SysSupervisor, &TCB_SysSupervisor);
//创建并运行机械动作任务
	xTaskCreate(vTask_Moving, "Moving", 128, NULL, uxPriority_Moving, &TCB_Moving);
//固化参数复原
	ParamSave_Load_pack();
//开启RTOS系统
	Printf_Chx(ChSW, "输入Help获取帮助，指令后需加新行\r\n");
	xTaskResumeAll();
	vTaskDelete(NULL);											//删除自己
}
void RTOS_TaskInit( void )
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);				//RTOS对应的中断配置需求
	xTaskCreate(vTask_StartOS, "StartOS", configMINIMAL_STACK_SIZE * 3, NULL, configMAX_PRIORITIES- 1, NULL);
	vTaskStartScheduler();										//开启任务调度器，开始Os系统初始化
}
