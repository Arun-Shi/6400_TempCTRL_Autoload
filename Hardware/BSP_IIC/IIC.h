#ifndef __IIC_H__
#define __IIC_H__
#include "Base_Head_File.h"

#define IIC_EXP_ADDR	0xE6											//定义IIC 1分4拓展 的IIC地址

#define STA_CHECK(x) do{if((x) && !status) status =index; else index++;}while(0);		//如果status=0，且x结果为非0，则此次操作状态异常，将Index赋给Status保存，1、若status已经为非0，前提条件不成立，index++（无意义）2、若x结果为0，则此次无异常，index++（索引递增以标记通讯阶段）

//IIC所有操作函数
void MyIIC_Init(void);               					//初始化IIC的IO口				 
u8 PCA954X_Select_Line(u8 Addr, u8 Line);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_SendAck(u8 Ack);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
void Scan_IIC(void);
u8 Addr_Chek(u8 Addr);

/*****************************OS环境的IIC接口 */
// 通用事务接口 - 适配所有器件
u8 IIC_Write_Ex(u8 DevAddr, u16 RegAddr, u8 RegLen, u8* pData, u16 DataLen);
u8 IIC_Read_Ex(u8 DevAddr, u16 RegAddr, u8 RegLen, u8* pBuf, u16 DataLen);

// 辅助功能
u8 IIC_Bus_BusyCheck(void);

#define _IIC_RegLen0     0
#define _IIC_RegLen8     1
#define _IIC_RegLen16    2
#define _IIC_RegLen32    4



#endif

