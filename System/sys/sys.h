#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"
 
//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持UCOS
																	    
//常量宏
#define Once_byte	1
#define Double_byte	2
#define Triple_byte	3

//以下为汇编函数
void WFI_SET(void);		     //执行WFI指令
void INTX_DISABLE(void);  //关闭所有中断
void INTX_ENABLE(void);	 //开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址

#endif
