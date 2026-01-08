#ifndef __EEP_H__
#define __EEP_H__

#include "Base_Head_File.h"

typedef enum{
	EEP_C01=1,	EEP_C02=2,	EEP_C04=4,	EEP_C08=8,	EEP_C16=16,	EEP_C32=32,	EEP_C64=64,	EEP_C128=128,	EEP_C256=256,	EEP_C512=512,	EEP_C1024=1024
}EEP_Size;

u16 EEP_PageSize(EEP_Size Chip);
u8 EEP_R_nByte(u8 Devic_Addr, u16 Reg_Addr, u8* Buff, u32 size);
u8 EEP_R_1Byte(u8 DAddr, u16 RAddr, u8* Buff);
u8 EEP_W_1Byte(u8 Devic_Addr, u16 Reg_Addr, u8 Data);
u8 EEP_W_nByte(u8 Devic_Addr, u16 Reg_Addr, u8* Data, u32 Size);

void EEPROM_Read(u8 Exp_Ch, u8 Devic_Addr,u16 ReadAddr,u16 NumToRead);
void EEPROM_Write(u8 Ch, u8 DeviceAddr, u16 WriteAddr, u8 *pBuffer, u16 NumToWrite);

//符合os系统的接口函数
u8 EEP_Read_Ex(u8 DevAddr, u16 RegAddr, u8* pBuf, u32 Size);
u8 EEP_Write_Ex(u8 DevAddr, u16 RegAddr, u8* pData, u32 Size);

#endif

