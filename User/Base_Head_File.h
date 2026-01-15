#ifndef _BASE_HEAD_FILE_H
#define _BASE_HEAD_FILE_H

/**************************************C库****************************************************/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**************************************FreeRTOS中间件****************************************************/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "list.h"
#include "portmacro.h"
#include "event_groups.h"
#include "semphr.h"
/**************************************STM32库****************************************************/
#include "stm32f10x.h"
/***************************************驱动层***************************************************/
//底层，相互不包含
#include "./MY_DEF.h"
#include "./sys/sys.h"
#include "./delay/delay.h"
#include "./Smp_IO/Smp_IO.h"
#include "./usart/usart.h" 
#include "./Command/Command.h"
#include "./BSP_IIC/IIC.H"

//中层，可能包含底层
#include "./M24Cxx/M24Cxx.h"
#include "./ParamSave/ParamSave.h"
#include "./PWM/PWM.h"
#include "./PID/PID.h"
#include "./DS18B20/DS18B20.h"

/*******************************************业务层代码***********************************************/
#include "RTOS_Task.h"
#include "./Main_Func/Main_Func.h"

#endif
