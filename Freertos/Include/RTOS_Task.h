#ifndef _RTOS_TASK_H
#define _RTOS_TASK_H

#include "./Base_Head_File.h"

void RTOS_TaskInit(void );
void vTask_MyIdle( void* arg );
void vTask_CycleSendElecData(void* arg);

extern SemaphoreHandle_t xMutex_UartTX;									//打印端口的互斥量
extern SemaphoreHandle_t xMutex_IICBus;									//IIC总线互斥量
extern SemaphoreHandle_t xSem_UartRx;									//接收端口的互斥量
#endif /* _RTOS_TASK_H */
